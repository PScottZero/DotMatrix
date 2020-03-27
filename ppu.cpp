#include "ppu.h"
#include <qpainter.h>

PPU::PPU(unsigned char *cpuMem, unsigned int *cpuClock, QImage *frame) {
    mem = cpuMem;
    clk = cpuClock;
    display = frame;
    ppuMode = Mode::MODE_2;
    currLine = 0;

    mem[SCROLL_X] = 0;
    mem[SCROLL_Y] = 0;
    mem[LCDC_Y] = 0;
}

void PPU::step() {
    currLine = (*clk % SCREEN_CYCLE) / SCANLINE + 1;
    if (currLine != mem[LCDC_Y] && currLine == 1) {
        mem[LCDC_Y] = 0;
    }
    if (currLine <= SCREEN_HEIGHT) {
        ppuMode = Mode::MODE_3;
        if (currLine != mem[LCDC_Y]) {
            mem[LCDC_Y]++;
            render();
        }
    } else {
        ppuMode = Mode::MODE_1;
        if (currLine != mem[LCDC_Y]) {
            mem[LCDC_Y]++;
            render();
        }
    }
}

void PPU::render() {
    switch (ppuMode) {

    // draw sprites
    case Mode::MODE_2:
        break;

    // vertical blank
    case Mode::MODE_1:
        mem[LCDC_Y]++;
        break;

    // horizontal blank
    case Mode::MODE_0:
        break;

    // draw background
    case Mode::MODE_3:

        // scroll registers
        int scrollY = mem[0xFF42];
        int scrollX = mem[0xFF43];

        // background addresses
        unsigned short bgDataAddr = BG_DATA_ADDR_0;
        unsigned short bgMapAddr = BG_WIN_MAP_ADDR_0;

        if (bgWinDataSelect() == 1) {
            bgDataAddr = BG_DATA_ADDR_1;
        }

        if (bgMapSelect() == 1) {
            bgMapAddr = BG_WIN_MAP_ADDR_1;
        }

        // render line
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            unsigned short tileMapOffset = (((scrollX + i) % BG_PX_DIM) / TILE_PX_DIM) +
                    (((scrollY + mem[LCDC_Y]) % BG_PX_DIM) / TILE_PX_DIM) * BG_TILE_DIM;
            unsigned short tileOffset = mem[bgMapAddr + tileMapOffset];
            unsigned short tileAddr = bgDataAddr + tileOffset * BYTES_PER_TILE + (scrollY + mem[LCDC_Y]) % TILE_PX_DIM;
            unsigned char byte0 = mem[tileAddr];
            unsigned char byte1 = mem[tileAddr + 1];

            unsigned char bit1 = (byte0 >> (7 - ((scrollX + i) % TILE_PX_DIM))) & 1;
            unsigned char bit0 = (byte1 >> (7 - ((scrollX + i) % TILE_PX_DIM))) & 1;

            unsigned char color = (bit1 << 1) | bit0;

            display->setPixel(i, mem[LCDC_Y] - 1, getPixelColor(color));
        }
        break;
    }
}



// -------------------- LCD CONTROL --------------------

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



// -------------------- BG PALETTE DATA --------------------

uint PPU::getPixelColor(unsigned char value) {
    switch (value) {
    case PX_ZERO:
        return getColor(mem[0xFF47] & 3);
    case PX_ONE:
        return getColor((mem[0xFF47] >> 2) & 3);
    case PX_TWO:
        return getColor((mem[0xFF47] >> 4) & 3);
    case PX_THREE:
        return getColor((mem[0xFF47] >> 6) & 3);
    }
    return 0;
}

uint PPU::getColor(unsigned char value) {
    switch (value) {
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

