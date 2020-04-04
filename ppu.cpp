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

    // draw background
    case Mode::MODE_3:
        mem[LCDC_Y]++;

        // scroll registers
        int scrollY = mem[0xFF42];
        int scrollX = mem[0xFF43];

        // background addresses
        unsigned short bgDataAddr = BG_WIN_DATA_ADDR_0;
        unsigned short bgMapAddr = BG_MAP_ADDR_0;

        if (bgWinDataSelect() == 1) {
            bgDataAddr = BG_WIN_DATA_ADDR_1;
        }

        if (bgMapSelect() == 1) {
            bgMapAddr = BG_MAP_ADDR_1;
        }

        // render line
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            unsigned short dispX = (scrollX + i) % BG_PX_DIM;
            unsigned short dispY = (scrollY + mem[LCDC_Y] - 1) % BG_PX_DIM;

            unsigned short tileMapOffset = dispX / TILE_PX_DIM + (dispY / TILE_PX_DIM) * BG_TILE_DIM;
            unsigned short tileOffset = mem[bgMapAddr + tileMapOffset];
            unsigned short tileRowAddr = bgDataAddr + tileOffset * BYTES_PER_TILE + (dispY % TILE_PX_DIM) * 2;

            unsigned char bit1 = (mem[tileRowAddr] >> (7 - ((scrollX + i) % TILE_PX_DIM))) & 1;
            unsigned char bit0 = (mem[tileRowAddr + 1] >> (7 - ((scrollX + i) % TILE_PX_DIM))) & 1;

            display->setPixel(i, mem[LCDC_Y] - 1, getPixelColor((bit1 << 1) | bit0));
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
uint PPU::getPixelColor(unsigned char value) {
    unsigned char color;

    // get map of color value
    switch (value) {
    case PX_ZERO:
        color = mem[0xFF47] & 3;
        break;
    case PX_ONE:
        color = (mem[0xFF47] >> 2) & 3;
        break;
    case PX_TWO:
        color = (mem[0xFF47] >> 4) & 3;
        break;
    case PX_THREE:
        color = (mem[0xFF47] >> 6) & 3;
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
    }
    return 0;
}



// ======================================================================================
// ------------------------------ LCD CONTROL FUNCTIONS ---------------------------------
// ======================================================================================

int PPU::lcdDisplayEnable() {
    return (mem[0xFF40] >> 7) & 1;
}

int PPU::windowMapSelect() {
    return (mem[0xFF40] >> 6) & 1;
}

int PPU::windowDisplayEnable() {
    return (mem[0xFF40] >> 5) & 1;
}

int PPU::bgWinDataSelect() {
    return (mem[0xFF40] >> 4) & 1;
}

int PPU::bgMapSelect() {
    return (mem[0xFF40] >> 3) & 1;
}

int PPU::spriteSize() {
    return (mem[0xFF40] >> 2) & 1;
}

int PPU::spriteEnable() {
    return (mem[0xFF40] >> 1) & 1;
}

int PPU::bgDisplayEnable() {
    return mem[0xFF40] & 1;
}



// ======================================================================================
// ------------------------------- INTERRUPT FUNCTIONS ----------------------------------
// ======================================================================================

void PPU::setLcdInt() {
    mem[IF] &= 0xFF & ((mem[LCDC_Y] == mem[LY_COMP]) << 1);
}

void PPU::setVblankInt() {
    mem[IF] &= 0xFF & (mem[LCDC_Y] == 144);
}
