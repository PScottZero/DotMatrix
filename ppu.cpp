#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "ppu.h"
#include <qpainter.h>

// ================================
// Initialize ppu data
// ================================
PPU::PPU(unsigned char *cpuMem, unsigned int *cpuClock, QImage *frame) {
    mem = cpuMem;
    mem[LY] = 0;
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
    if (lcdDisplayEnable()) {
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
                    render();
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
// Render display cycle
// ================================
void PPU::render() {
    rendered = true;
    switch (mem[LCD_STAT] & 0x3) {

    // draw sprites
    case MODE_2:
        break;

    case MODE_3:
        auto *scanline = new unsigned char[160];
        auto *scanlinePal = new unsigned short[160];

        // draw background
        if (bgDisplayEnable()) {

            // background addresses
            unsigned short bgDataAddr = DATA_ADDR_0;
            unsigned short bgMapAddr = MAP_ADDR_0;

            if (bgWinDataSelect() == 1) {
                bgDataAddr = DATA_ADDR_1;
            }

            if (bgMapSelect() == 1) {
                bgMapAddr = MAP_ADDR_1;
            }

            // render line
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                unsigned short tileX = (mem[SCROLL_X] + i) % BG_PX_DIM;
                unsigned short tileY = (mem[SCROLL_Y] + mem[LY]) % BG_PX_DIM;

                unsigned short tileMapOffset = tileX / TILE_PX_DIM + (tileY / TILE_PX_DIM) * BG_TILE_DIM;
                unsigned char tileNo = mem[bgMapAddr + tileMapOffset];
                unsigned short tileRowAddr;
                if (bgWinDataSelect() == 1) {
                    tileRowAddr = bgDataAddr + tileNo * BYTES_PER_TILE + (tileY % TILE_PX_DIM) * 2;
                } else {
                    tileRowAddr = 0x9000 + (char)tileNo * BYTES_PER_TILE + (tileY % TILE_PX_DIM) * 2;
                }

                unsigned char bit0 = (mem[tileRowAddr] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;
                unsigned char bit1 = (mem[tileRowAddr + 1] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;

                scanline[i] = (bit1 << 1) | bit0;
                scanlinePal[i] = BGP;
            }
        } else {
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                scanline[i] = 0;
                scanlinePal[i] = BGP;
            }
        }

        // draw sprites
        if (spriteEnable()) {

            // find sprites in scanline
            for (int oamEntry = 0; oamEntry < OAM_COUNT; oamEntry++) {
                unsigned posY = getSpriteY(oamEntry);

                // 8x8 mode
                if (!spriteSize()) {
                    if (posY <= mem[LY] + 0x10 && posY > mem[LY] + 0x8) {
                        unsigned posX = getSpriteX(oamEntry);
                        unsigned tileNo = getSpriteTileNo(oamEntry);
                        unsigned rowNo = mem[LY] - (posY - 0x10);
                        if (spriteFlipY(oamEntry)) {
                            rowNo = 7 - rowNo;
                        }

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
                                        (scanline[scanIndex] == 0 && scanlinePal[scanIndex] == BGP)) {
                                        scanline[scanIndex] = pxVal;
                                        if (getSpritePalette(oamEntry)) {
                                            scanlinePal[scanIndex] = OBP1;
                                        } else {
                                            scanlinePal[scanIndex] = OBP0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    if (posY <= mem[LY] + 0x10 && posY > mem[LY]) {
                        unsigned posX = getSpriteX(oamEntry);
                        unsigned tileNo = getSpriteTileNo(oamEntry);
                        unsigned rowNo = mem[LY] - (posY - 0x10);
                    }
                }
            }
        }

        // transfer scanline to display
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            uint color = getPixelColor(scanline[i], scanlinePal[i]);
            if (scanlinePal[i] == BGP && !bgDisplayEnable()) {
                color = 0xFFFFFF;
            }
            display->setPixel(i, mem[LY], color);
        }
        break;
    }
    setLcdInt();
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
            return 0xFFFFFF;
        case PX_ONE:
            return 0xAAAAAA;
        case PX_TWO:
            return 0x555555;
        case PX_THREE:
            return 0x000000;
        default:
            break;
    }
    return 0;
}



// ======================================================================================
// ------------------------------ LCD CONTROL FUNCTIONS ---------------------------------
// ======================================================================================

int PPU::lcdDisplayEnable() const {
    return (mem[LCDC] >> 7) & 1;
}

int PPU::windowMapSelect() const {
    return (mem[LCDC] >> 6) & 1;
}

int PPU::windowDisplayEnable() const {
    return (mem[LCDC] >> 5) & 1;
}

int PPU::bgWinDataSelect() const {
    return (mem[LCDC] >> 4) & 1;
}

int PPU::bgMapSelect() const {
    return (mem[LCDC] >> 3) & 1;
}

int PPU::spriteSize() const {
    return (mem[LCDC] >> 2) & 1;
}

int PPU::spriteEnable() const {
    return (mem[LCDC] >> 1) & 1;
}

int PPU::bgDisplayEnable() const {
    return mem[LCDC] & 1;
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