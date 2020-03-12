#include "ppu.h"
#include <qpainter.h>

PPU::PPU(QMainWindow* dotMatrixClass) {
    dm = dotMatrixClass;
    mem = NULL;

    // game boy pocket color palette
    white = QBrush(QColor(255, 255, 255));
    lightGray = QBrush(QColor(169, 169, 169));
    darkGray = QBrush(QColor(84, 84, 84));
    black = QBrush(QColor(0, 0, 0));

    //// original game boy color palette
    //white = QBrush(QColor(155, 188, 15));
    //lightGray = QBrush(QColor(139, 172, 15));
    //darkGray = QBrush(QColor(48, 98, 48));
    //black = QBrush(QColor(15, 56, 15));

    // create vram
    initVideo();
}

void PPU::setMemory(unsigned char* cpuMem) {
    mem = cpuMem;
}

void PPU::initVideo() {
    vram = new pixel*[BG_PX_DIM];
    for (int i = 0; i < BG_PX_DIM; i++) {
        vram[i] = new pixel[BG_PX_DIM];
        for (int j = 0; j < BG_PX_DIM; j++) {
            vram[i][j] = {PX_THREE, 0, PixelType::Background};
        }
    }
}

// Renders display
// TODO: implement render function
void PPU::render() {
    if (bgDisplayEnable() == 1) {
        setBackgroundTiles();
    }
    update();
}

// takes background map and copies
// corresponding tiles into video ram
void PPU::setBackgroundTiles() {
    unsigned short bgDataAddr = BG_DATA_ADDR_0;
    unsigned short bgMapAddr = BG_WIN_MAP_ADDR_0;

    if (bgWinDataSelect() == 1) {
        bgDataAddr = BG_DATA_ADDR_1;
    }

    if (bgMapSelect() == 1) {
        bgMapAddr = BG_WIN_MAP_ADDR_1;
    }

    // iterate through tile map
    for (int index = 0; index < BG_TILE_COUNT; index++) {
        unsigned short tileAddr = bgDataAddr + BYTES_PER_TILE * mem[bgMapAddr + index];
        int rowOffset = (index / BG_TILE_DIM) * TILE_PX_DIM;
        int colOffset = (index % BG_TILE_DIM) * TILE_PX_DIM;

        // write tile to VRAM
        for (int row = 0; row < TILE_PX_DIM; row++) {

            // get row data
            unsigned char byte0 = mem[tileAddr + 2 * row];
            unsigned char byte1 = mem[tileAddr + 2 * row + 1];

            for (int col = 0; col < TILE_PX_DIM; col++) {

                // get pixel data for tile
                unsigned char bit1 = (byte0 >> (7 - col)) & 1;
                unsigned char bit0 = (byte1 >> (7 - col)) & 1;
                unsigned char color = (bit1 << 1) | bit0;
                vram[rowOffset + row][colOffset + col] = {color, 0, PixelType::Background};
            }
        }
    }
}

void PPU::paintEvent(QPaintEvent *e) {
    mem[0xFF44] = 0x90;
    if (mem != NULL) {
        QPainter painter(this);

        int scrollY = mem[0xFF42];
        int scrollX = mem[0xFF43];

        // set background palette
        QBrush colorZero = getShade(PX_ZERO);
        QBrush colorOne = getShade(PX_ONE);
        QBrush colorTwo = getShade(PX_TWO);
        QBrush colorThree = getShade(PX_THREE);

        for (int row = 0; row < BG_PX_DIM; row++) {
            for (int col = 0; col < BG_PX_DIM; col++) {
                pixel px = vram[(scrollY + row) % BG_PX_DIM][(scrollX + col) % BG_PX_DIM];
                if (px.type == PixelType::Background) {

                    QBrush pxColor;
                    switch (px.value) {
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

                    painter.fillRect(4 * (col - scrollX), 4 * (row - scrollY), 4, 4, pxColor);
                }
            }
        }
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

