#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "ppu.h"
#include <qpainter.h>

// ================================
// Initialize ppu data
// ================================
PPU::PPU(unsigned char *cpuMem, QImage *frame) {
    mem = cpuMem;
    display = frame;
    ppuCycle = 0;
    prevClock = 0;
    currClock = 0;
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
        ppuCycle += currClock - prevClock;
        ppuCycle %= SCREEN_CYCLE;
        mem[LY] = (ppuCycle / SCANLINE) % 154;
        if (ppuCycle / SCANLINE < 144) {
            mem[IF] &= 0xFE;
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
        mem[LCD_STAT] &= 0xFC;
    }
}

void PPU::setMode(Mode m) const {
    mem[LCD_STAT] &= 0xFC;
    mem[LCD_STAT] |= (unsigned char)m;
}

// ================================
// Draw scanline on display
// ================================
void PPU::drawScanline() {
    rendered = true;
    auto *scanline = new unsigned char[160];
    auto *palette = new unsigned short[160];

    // draw background
    if (bgEnable()) {
        drawBackground(scanline, palette);
    } else {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            scanline[i] = 0;
            palette[i] = BGP;
        }
    }

    // draw window
    if (windowEnable()) {
        drawWindow(scanline, palette);
    }

    // draw sprites
    if (spriteEnable()) {
        drawSprites(scanline, palette);
    }

    // transfer scanline to display
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        uint color = getPixelColor(scanline[i], palette[i]);
        if (palette[i] == BGP && !bgEnable()) {
            color = 0xFFFFFF;
        }
        display->setPixel(i, mem[LY], color);
    }

    setLcdInt();
}

// ================================
// Draw row of background
// ================================
void PPU::drawBackground(unsigned char *scanline, unsigned short *palette) const {
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
        palette[i] = BGP;
    }
}

void PPU::drawWindow(unsigned char* scanline, unsigned short* palette) const {
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
                    palette[dispX] = BGP;
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
void PPU::drawSprites(unsigned char* scanline, unsigned short* palette) const {

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
                            (scanline[scanIndex] == 0 && palette[scanIndex] == BGP)) {
                            scanline[scanIndex] = pxVal;
                            if (getSpritePalette(oamEntry)) {
                                palette[scanIndex] = OBP1;
                            } else {
                                palette[scanIndex] = OBP0;
                            }
                        }
                    }
                }
            }
        }
    }
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

    // get color value
    switch (color) {
        case PX_ZERO:
            return 0xc5dffa;
        case PX_ONE:
            return 0x5c88b8;
        case PX_TWO:
            return 0x1a4778;
        case PX_THREE:
            return 0x00234a;
        default:
            break;
    }
    return 0;
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
    if (((mem[LY] == mem[LY_COMP]) && (mem[LCD_STAT] & 0x40)) ||
        (((mem[LCD_STAT] & 0x03) == 0) && (mem[LCD_STAT] & 0x08)) ||
        (((mem[LCD_STAT] & 0x03) == 2) && (mem[LCD_STAT] & 0x20)) ||
        (((mem[LCD_STAT] & 0x03) == 3) && (mem[LCD_STAT] & 0x10) && (mem[LCD_STAT] & 0x20))) {
        mem[IF] |= 0b10;
    }
}

void PPU::triggerVBlankInt() const {
    mem[IF] |= 0b01;
}

#pragma clang diagnostic pop