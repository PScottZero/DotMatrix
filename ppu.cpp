#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "ppu.h"

// ================================
// Initialize ppu data
// ================================
PPU::PPU(unsigned char *cpuMem, unsigned int* cpuCycleCount) {
    mem = cpuMem;
    display = nullptr;
    palette = new Palette(0xFFFFFF, 0xAAAAAA, 0x555555, 0x000000);
    ppuCycle = 0;
    cycleCount = cpuCycleCount;
    rendered = false;
}



// ======================================================================================
// -------------------------------- DISPLAY FUNCTIONS -----------------------------------
// ======================================================================================

// ================================
// Step through one display cycle
// ================================
void PPU::step() {
    if (lcdEnable()) {
        ppuCycle += *cycleCount;
        ppuCycle %= SCREEN_CYCLE;
        mem[LY] = (ppuCycle / SCANLINE) % 154;
        if (mem[LY] == mem[LY_COMP]) {
            mem[STAT] |= 0x04;
        } else {
            mem[STAT] &= 0xFB;
        }
        setLcdInt();
        if (ppuCycle / SCANLINE < 144) {
            if (ppuCycle % SCANLINE <= 20) {
                rendered = false;
                setMode(MODE_2);
            } else if (ppuCycle % SCANLINE <= 63) {
                setMode(MODE_3);
                if (!rendered) {
                    drawScanline();
                }
            } else {
                rendered = false;
                setMode(MODE_0);
            }
        } else {
            if (mem[LY] == 144 && !rendered) {
                triggerVBlankInt();
                rendered = true;
            }
            setMode(MODE_1);
        }
    } else {
        mem[LY] = 0;
        setMode(MODE_0);
    }
}

void PPU::setMode(Mode m) const {
    mem[STAT] &= 0xFC;
    mem[STAT] |= (unsigned char)m;
}

// ================================
// Draw scanline on display
// ================================
void PPU::drawScanline() {
    rendered = true;
    auto *scanline = new unsigned char[160];
    auto *scanPal = new unsigned short[160];

    // draw background
    if (bgEnable()) {
        drawBackground(scanline, scanPal);
    } else {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            scanline[i] = 0;
            scanPal[i] = BGP;
        }
    }

    // draw window
    if (windowEnable()) {
        drawWindow(scanline, scanPal);
    }

    // draw sprites
    if (spriteEnable()) {
        drawSprites(scanline, scanPal);
    }

    // transfer scanline to display
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        uint color = getPixelColor(scanline[i], scanPal[i]);
        if (scanPal[i] == BGP && !bgEnable()) {
            color = palette->getColor(PX_ZERO);
        }
        display->setPixel(i, mem[LY], color);
    }

    delete[] scanline;
    delete[] scanPal;
}

// ================================
// Draw row of background
// ================================
void PPU::drawBackground(unsigned char *scanline, unsigned short *scanPal) const {
    unsigned short bgDataAddr = DATA_ADDR_0;
    unsigned short bgMapAddr = MAP_ADDR_0;

    if (bgWinDataSelect()) {
        bgDataAddr = DATA_ADDR_1;
    }

    if (bgMapSelect()) {
        bgMapAddr = MAP_ADDR_1;
    }

    // drawScanline line
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        unsigned short tileX = (mem[SCROLL_X] + i) % BG_PX_DIM;
        unsigned short tileY = (mem[SCROLL_Y] + mem[LY]) % BG_PX_DIM;

        unsigned short tileMapOffset = tileX / TILE_PX_DIM + (tileY / TILE_PX_DIM) * BG_TILE_DIM;
        unsigned char tileNo = mem[bgMapAddr + tileMapOffset];
        unsigned short tileRowAddr = bgDataAddr + tileNo * BYTES_PER_TILE + (tileY % TILE_PX_DIM) * 2;
        if (!bgWinDataSelect()) {
            tileRowAddr = bgDataAddr + (char)tileNo * BYTES_PER_TILE + (tileY % TILE_PX_DIM) * 2;
        }

        unsigned char bit0 = (mem[tileRowAddr] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;
        unsigned char bit1 = (mem[tileRowAddr + 1] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;

        scanline[i] = (bit1 << 1) | bit0;
        scanPal[i] = BGP;
    }
}

void PPU::drawWindow(unsigned char* scanline, unsigned short* scanPal) const {
    unsigned short winDataAddr = DATA_ADDR_0;
    unsigned short winMapAddr = MAP_ADDR_0;

    if (bgWinDataSelect()) {
        winDataAddr = DATA_ADDR_1;
    }

    if (windowMapSelect()) {
        winMapAddr = MAP_ADDR_1;
    }

    int winRow = mem[LY] - mem[WINDOW_Y];
    if (winRow >= 0 && winRow < 144) {
        for (int i = 0; i < BG_TILE_DIM; i++) {
            bool outOfBounds = false;
            auto tileNo = mem[winMapAddr + (winRow / TILE_PX_DIM) * BG_TILE_DIM + i];
            auto tileRowAddr = winDataAddr + tileNo * BYTES_PER_TILE + (winRow % TILE_PX_DIM) * 2;
            if (!bgWinDataSelect()) {
                tileRowAddr = winDataAddr + (char)tileNo * BYTES_PER_TILE + (winRow % TILE_PX_DIM) * 2;
            }
            unsigned char byte0 = mem[tileRowAddr];
            unsigned char byte1 = mem[tileRowAddr + 1];

            for (int j = 0; j < TILE_PX_DIM; j++) {
                int dispX = (mem[WINDOW_X] - 7) + i * TILE_PX_DIM + j;
                if (dispX >= 0 && dispX < 160) {
                    unsigned char bit0 = (byte0 >> (7 - j)) & 0x1;
                    unsigned char bit1 = (byte1 >> (7 - j)) & 0x1;
                    scanline[dispX] = (bit1 << 1) | bit0;
                    scanPal[dispX] = BGP;
                }

                if (dispX >= 160) {
                    outOfBounds = true;
                }
            }

            if (outOfBounds) {
                break;
            }
        }
    }
}

// ================================
// Draw row of sprites
// ================================
void PPU::drawSprites(unsigned char* scanline, unsigned short* scanPal) const {

    // find sprites in scanline
    for (int oamEntry = 0; oamEntry < OAM_COUNT; oamEntry++) {
        unsigned posY = getSpriteY(oamEntry);
        unsigned posX = getSpriteX(oamEntry);
        unsigned tileNo = 0;
        unsigned rowNo = 0;
        bool inScanline = false;

        // 8x8 mode
        if (!spriteSize()) {
            if (posY <= mem[LY] + 0x10 && posY > mem[LY] + 0x8) {
                inScanline = true;
                tileNo = getSpriteTileNo(oamEntry);
                rowNo = mem[LY] - (posY - 0x10);
                if (spriteFlipY(oamEntry)) {
                    rowNo = 7 - rowNo;
                }
            }
        } else {
            if (posY <= mem[LY] + 0x10 && posY > mem[LY]) {
                inScanline = true;
                tileNo = getSpriteTileNo(oamEntry) & 0xFE;
                rowNo = mem[LY] - (posY - 0x10);

                if (spriteFlipY(oamEntry)) {
                    rowNo = 0xF - rowNo;
                }
            }
        }

        // draw sprite
        if (inScanline) {
            unsigned char rowByte0 = mem[DATA_ADDR_1 + tileNo * BYTES_PER_TILE + rowNo * 2];
            unsigned char rowByte1 = mem[DATA_ADDR_1 + tileNo * BYTES_PER_TILE + rowNo * 2 + 1];

            for (int j = 0; j < 8; j++) {
                unsigned char scanIndex = (posX - 0x8) + j;
                if (scanIndex >= 0 && scanIndex < SCREEN_WIDTH) {
                    unsigned char bit1;
                    unsigned char bit0;
                    if (spriteFlipX(oamEntry)) {
                        bit0 = (rowByte0 >> j) & 0x1;
                        bit1 = (rowByte1 >> j) & 0x1;
                    } else {
                        bit0 = (rowByte0 >> (7 - j)) & 0x1;
                        bit1 = (rowByte1 >> (7 - j)) & 0x1;
                    }
                    unsigned char pxVal = (bit1 << 1) | bit0;
                    if (pxVal != 0) {
                        if (!spriteBehindBG(oamEntry) ||
                            (scanline[scanIndex] == 0 && scanPal[scanIndex] == BGP)) {
                            scanline[scanIndex] = pxVal;
                            if (getSpritePalette(oamEntry)) {
                                scanPal[scanIndex] = OBP1;
                            } else {
                                scanPal[scanIndex] = OBP0;
                            }
                        }
                    }
                }
            }
        }
    }
}

void PPU::setDisplay(QImage *disp) {
    display = disp;
}

void PPU::setPalette(Palette* pal) {
    palette = pal;
}

// ================================
// Get pixel color based on
// palette map
// ================================
uint PPU::getPixelColor(unsigned char value, unsigned short mapAddr) const {
    unsigned char color = 0;

    // get map of color value
    switch (value) {
        case PX_ZERO:
            color = mem[mapAddr] & 3;
            break;
        case PX_ONE:
            color = (mem[mapAddr] >> 2) & 3;
            break;
        case PX_TWO:
            color = (mem[mapAddr] >> 4) & 3;
            break;
        case PX_THREE:
            color = (mem[mapAddr] >> 6) & 3;
            break;
        default:
            break;
    }
    return palette->getColor(color);
}



// ======================================================================================
// ------------------------------ LCD CONTROL FUNCTIONS ---------------------------------
// ======================================================================================

bool PPU::lcdEnable() const {
    return mem[LCDC] & 0x80;
}

bool PPU::windowMapSelect() const {
    return mem[LCDC] & 0x40;
}

bool PPU::windowEnable() const {
    return mem[LCDC] & 0x20;
}

bool PPU::bgWinDataSelect() const {
    return mem[LCDC] & 0x10;
}

bool PPU::bgMapSelect() const {
    return mem[LCDC] & 0x08;
}

bool PPU::spriteSize() const {
    return mem[LCDC] & 0x04;
}

bool PPU::spriteEnable() const {
    return mem[LCDC] & 0x02;
}

bool PPU::bgEnable() const {
    return mem[LCDC] & 0x01;
}



// ======================================================================================
// ---------------------------------- OAM FUNCTIONS -------------------------------------
// ======================================================================================

unsigned char PPU::getSpriteY(int oamEntry) const {
    return mem[OAM_ADDR + 4 * oamEntry];
}

unsigned char PPU::getSpriteX(int oamEntry) const {
    return mem[OAM_ADDR + 4 * oamEntry + 1];
}

unsigned char PPU::getSpriteTileNo(int oamEntry) const {
    return mem[OAM_ADDR + 4 * oamEntry + 2];
}

bool PPU::spriteBehindBG(int oamEntry) const {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 7) & 1;
}

bool PPU::spriteFlipY(int oamEntry) const {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 6) & 1;
}

bool PPU::spriteFlipX(int oamEntry) const {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 5) & 1;
}

bool PPU::getSpritePalette(int oamEntry) const {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 4) & 1;
}



// ======================================================================================
// ------------------------------- INTERRUPT FUNCTIONS ----------------------------------
// ======================================================================================

void PPU::setLcdInt() const {
    if (coincidenceCheck() ||
        modeCheck(MODE_0, 0x8) ||
        modeCheck(MODE_1, 0x10) ||
        modeCheck(MODE_2, 0x20)) {
        mem[IF] |= 0b10;
    }
}

void PPU::triggerVBlankInt() const {
    mem[IF] |= 0b01;
}

bool PPU::coincidenceCheck() const {
    return (mem[STAT] & 0x04) && (mem[STAT] & 0x40);
}

bool PPU::modeCheck(Mode mode, unsigned char mask) const {
    return ((mem[STAT] & 0x3) == mode) && (mem[STAT] & mask);
}

#pragma clang diagnostic pop
