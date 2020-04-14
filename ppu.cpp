#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "ppu.h"
#include <qpainter.h>

// ================================
// Initialize ppu data
// ================================
PPU::PPU(unsigned char *cpuMem, unsigned int *cpuClock, QImage *frame) {
    mem = cpuMem;
    clk = cpuClock;
    display = frame;
    currLine = 0;
}



// ======================================================================================
// -------------------------------- DISPLAY FUNCTIONS -----------------------------------
// ======================================================================================

// ================================
// Step through one display cycle
// ================================
void PPU::step() {
    currLine = (*clk % SCREEN_CYCLE) / SCANLINE + 1;
    if (currLine != mem[LCDC_Y] && currLine == 1) {
        mem[LCDC_Y] = 0;
    }
    if (currLine <= SCREEN_HEIGHT) {
        mem[LCD_STAT] = (unsigned char) Mode::MODE_3;
        if (currLine != mem[LCDC_Y]) {
            render();
        }
    } else {
        mem[LCD_STAT] = (unsigned char) Mode::MODE_1;
        if (currLine != mem[LCDC_Y]) {
            render();
        }
    }
}

// ================================
// Render display cycle
// ================================
void PPU::render() {
    switch ((Mode)mem[LCD_STAT]) {

    // draw sprites
    case Mode::MODE_2:
        break;

    // vertical blank
    case Mode::MODE_1:
        mem[LCDC_Y]++;
        setVblankInt();
        break;

    // horizontal blank
    case Mode::MODE_0:
        break;

    case Mode::MODE_3:
        mem[LCDC_Y]++;

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
                unsigned short dispX = (mem[SCROLL_X] + i) % BG_PX_DIM;
                unsigned short dispY = (mem[SCROLL_Y] + mem[LCDC_Y] - 1) % BG_PX_DIM;

                unsigned short tileMapOffset = dispX / TILE_PX_DIM + (dispY / TILE_PX_DIM) * BG_TILE_DIM;
                unsigned short tileOffset = mem[bgMapAddr + tileMapOffset];
                unsigned short tileRowAddr = bgDataAddr + tileOffset * BYTES_PER_TILE + (dispY % TILE_PX_DIM) * 2;

                unsigned char bit1 = (mem[tileRowAddr] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;
                unsigned char bit0 = (mem[tileRowAddr + 1] >> (7 - ((mem[SCROLL_X] + i) % TILE_PX_DIM))) & 1;

                scanline[i] = (bit1 << 1) | bit0;
                scanlinePal[i] = BGP;
            }
        }

        // draw sprites
        if (spriteEnable()) {

            // find sprites in scanline
            for (int oamEntry = 0; oamEntry < OAM_COUNT; oamEntry++) {

                // 8x8 mode
                if (!spriteSize()) {
                    unsigned posY = getSpriteY(oamEntry);
                    if (posY <= mem[LCDC_Y] + 0xF && posY > mem[LCDC_Y] + 0x7) {
                        unsigned posX = getSpriteX(oamEntry);
                        unsigned tileNo = getSpriteTileNo(oamEntry);
                        unsigned rowNo = mem[LCDC_Y] - (posY - 0xF);
                        if (spriteFlipY(oamEntry)) {
                            rowNo = 7 - rowNo;
                        }

                        unsigned char rowByte1 = mem[DATA_ADDR_1 + tileNo * BYTES_PER_TILE + rowNo * 2];
                        unsigned char rowByte0 = mem[DATA_ADDR_1 + tileNo * BYTES_PER_TILE + rowNo * 2 + 1];

                        for (int j = 0; j < 8; j++) {
                            unsigned char scanIndex = (posX - 0x8) + j;
                            if (scanIndex >= 0 && scanIndex < SCREEN_WIDTH) {
                                unsigned char bit1;
                                unsigned char bit0;
                                if (spriteFlipX(oamEntry)) {
                                    bit1 = (rowByte1 >> j) & 0x1;
                                    bit0 = (rowByte0 >> j) & 0x1;
                                } else {
                                    bit1 = (rowByte1 >> (7 - j)) & 0x1;
                                    bit0 = (rowByte0 >> (7 - j)) & 0x1;
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
                }
            }
        }

        // transfer scanline to display
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            display->setPixel(i, mem[LCDC_Y] - 1, getPixelColor(scanline[i], scanlinePal[i]));
        }
        break;
    }

    setLcdInt();
    setVblankInt();
}

// ================================
// Get pixel color based on
// palette map
// ================================
uint PPU::getPixelColor(unsigned char value, unsigned short mapAddr) {
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

int PPU::lcdDisplayEnable() {
    return (mem[LCDC] >> 7) & 1;
}

int PPU::windowMapSelect() {
    return (mem[LCDC] >> 6) & 1;
}

int PPU::windowDisplayEnable() {
    return (mem[LCDC] >> 5) & 1;
}

int PPU::bgWinDataSelect() {
    return (mem[LCDC] >> 4) & 1;
}

int PPU::bgMapSelect() {
    return (mem[LCDC] >> 3) & 1;
}

int PPU::spriteSize() {
    return (mem[LCDC] >> 2) & 1;
}

int PPU::spriteEnable() {
    return (mem[LCDC] >> 1) & 1;
}

int PPU::bgDisplayEnable() {
    return mem[LCDC] & 1;
}



// ======================================================================================
// ---------------------------------- OAM FUNCTIONS -------------------------------------
// ======================================================================================

unsigned char PPU::getSpriteY(int oamEntry) {
    return mem[OAM_ADDR + 4 * oamEntry];
}

unsigned char PPU::getSpriteX(int oamEntry) {
    return mem[OAM_ADDR + 4 * oamEntry + 1];
}

unsigned char PPU::getSpriteTileNo(int oamEntry) {
    return mem[OAM_ADDR + 4 * oamEntry + 2];
}

bool PPU::spriteBehindBG(int oamEntry) {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 7) & 1;
}

bool PPU::spriteFlipY(int oamEntry) {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 6) & 1;
}

bool PPU::spriteFlipX(int oamEntry) {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 5) & 1;
}

bool PPU::getSpritePalette(int oamEntry) {
    return (mem[OAM_ADDR + 4 * oamEntry + 3] >> 4) & 1;
}



// ======================================================================================
// ------------------------------- INTERRUPT FUNCTIONS ----------------------------------
// ======================================================================================

void PPU::setLcdInt() {
    if (mem[LCDC_Y] == mem[LY_COMP]) {
        mem[IF] |= 0b10;
    } else {
        mem[IF] &= 0xFD;
    }
}

void PPU::setVblankInt() {
    if (mem[LCDC_Y] == 145) {
        mem[IF] |= 0b01;
    } else {
        mem[IF] &= 0xFE;
    }
}

#pragma clang diagnostic pop