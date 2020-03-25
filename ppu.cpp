#include "ppu.h"
#include <qpainter.h>

PPU::PPU(QMainWindow* dotMatrixClass) {
    dm = dotMatrixClass;
    mem = NULL;
    cpuClock = 0;
    ppuMode = Mode::MODE_2;
    currLine = 0;

    // game boy pocket color palette
    white = QBrush(QColor(255, 255, 255));
    lightGray = QBrush(QColor(169, 169, 169));
    darkGray = QBrush(QColor(84, 84, 84));
    black = QBrush(QColor(0, 0, 0));

//    // original game boy color palette
//    white = QBrush(QColor(155, 188, 15));
//    lightGray = QBrush(QColor(139, 172, 15));
//    darkGray = QBrush(QColor(48, 98, 48));
//    black = QBrush(QColor(15, 56, 15));
}

void PPU::setMemory(unsigned char* cpuMem) {
    mem = cpuMem;
    mem[SCROLL_X] = 0;
    mem[SCROLL_Y] = 0;
    mem[LCDC_Y] = 0;
}

void PPU::setCPUClock(unsigned int* clk) {
    cpuClock = clk;
}

void PPU::step() {
    currLine = (*cpuClock % SCREEN_CYCLE) / SCANLINE + 1;
    auto lcdc = mem[LCDC_Y];
    if (currLine != mem[LCDC_Y] && currLine == 1) {
        mem[LCDC_Y] = 0;
    }
    if (currLine <= SCREEN_HEIGHT) {
        ppuMode = Mode::MODE_3;
        if (currLine != mem[LCDC_Y]) {
            mem[LCDC_Y]++;
            repaint(QRect(0, mem[LCDC_Y] * 4, SCREEN_WIDTH * 4, 4));
        }
    } else {
        ppuMode = Mode::MODE_1;
        if (currLine != mem[LCDC_Y]) {
            mem[LCDC_Y]++;
            repaint(QRect(0, mem[LCDC_Y] * 4, SCREEN_WIDTH * 4, 4));
        }
    }
}

void PPU::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
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

        // background palette
        QBrush colorZero = getShade(PX_ZERO);
        QBrush colorOne = getShade(PX_ONE);
        QBrush colorTwo = getShade(PX_TWO);
        QBrush colorThree = getShade(PX_THREE);

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

            QBrush pxColor;

            switch (color) {
            case PX_ZERO:
                pxColor = colorZero;
                break;
            case PX_ONE:
                pxColor = colorOne;
                break;
            case PX_TWO:
                pxColor = colorTwo;
                break;
            case PX_THREE:
                pxColor = colorThree;
                break;
            }

            painter.fillRect(4 * i, 4 * (mem[LCDC_Y] - 1), 4, 4, pxColor);
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

QBrush PPU::getShade(unsigned char value) {
    switch (value) {
    case PX_ZERO:
        return getBrush(mem[0xFF47] & 3);
    case PX_ONE:
        return getBrush((mem[0xFF47] >> 2) & 3);
    case PX_TWO:
        return getBrush((mem[0xFF47] >> 4) & 3);
    case PX_THREE:
        return getBrush((mem[0xFF47] >> 6) & 3);
    }
}

QBrush PPU::getBrush(unsigned char value) {
    switch (value) {
    case PX_ZERO:
        return white;
    case PX_ONE:
        return lightGray;
    case PX_TWO:
        return darkGray;
    case PX_THREE:
        return black;
    }
}

