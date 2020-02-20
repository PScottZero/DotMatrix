#include "GB_PPU.h"
#include <qpainter.h>

GB_PPU::GB_PPU(QMainWindow* dotMatrixClass) {
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

void GB_PPU::setMemory(unsigned char* cpuMem) {
    mem = cpuMem;
}

void GB_PPU::setMutex(mutex* mutex) {
    rw = mutex;
}

void GB_PPU::initVideo() {
	vram = new pixel*[BG_PX_DIM];
	for (int i = 0; i < BG_PX_DIM; i++) {
		vram[i] = new pixel[BG_PX_DIM];
		for (int j = 0; j < BG_PX_DIM; j++) {
			vram[i][j] = {THREE, 0, PixelType::Background};
		}
	}
}

// Renders display
// TODO: implement render function
void GB_PPU::render() {
	if (bgDisplayEnable() == 1) {
		setBackgroundTiles();
	}
    update();
}

// takes background map and copies
// corresponding tiles into video ram
void GB_PPU::setBackgroundTiles() {
    rw->lock();
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
    rw->unlock();
}

void GB_PPU::paintEvent(QPaintEvent *e) {
    if (mem != NULL) {
        QPainter painter(this);

        int scrollY = mem[0xFF42];
        int scrollX = mem[0xFF43];

        // set background palette
        QBrush colorZero = getShade(ZERO);
        QBrush colorOne = getShade(ONE);
        QBrush colorTwo = getShade(TWO);
        QBrush colorThree = getShade(THREE);

        for (int row = 0; row < BG_PX_DIM; row++) {
            rw->lock();
            mem[0xFF44] = row;
            rw->unlock();
            for (int col = 0; col < BG_PX_DIM; col++) {
                pixel px = vram[(scrollY + row) % BG_PX_DIM][(scrollX + col) % BG_PX_DIM];
                if (px.type == PixelType::Background) {

                    QBrush pxColor;
                    switch (px.value) {
                    case ZERO:
                        pxColor = colorZero;
                        break;
                    case ONE:
                        pxColor = colorOne;
                        break;
                    case TWO:
                        pxColor = colorTwo;
                        break;
                    case THREE:
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

int GB_PPU::lcdDisplayEnable() {
    return (mem[0xFF40] >> 7) & 1;
}

int GB_PPU::windowMapSelect() {
    return (mem[0xFF40] >> 6) & 1;
}

int GB_PPU::windowDisplayEnable() {
    return (mem[0xFF40] >> 5) & 1;
}

int GB_PPU::bgWinDataSelect() {
    return (mem[0xFF40] >> 4) & 1;
}

int GB_PPU::bgMapSelect() {
    return (mem[0xFF40] >> 3) & 1;
}

int GB_PPU::spriteSize() {
    return (mem[0xFF40] >> 2) & 1;
}

int GB_PPU::spriteEnable() {
    return (mem[0xFF40] >> 1) & 1;
}

int GB_PPU::bgDisplayEnable() {
    return mem[0xFF40] & 1;
}



// -------------------- BG PALETTE DATA --------------------

QBrush GB_PPU::getShade(unsigned char value) {
    switch (value) {
    case ZERO:
        return getBrush(mem[0xFF47] & 3);
    case ONE:
        return getBrush((mem[0xFF47] >> 2) & 3);
    case TWO:
        return getBrush((mem[0xFF47] >> 4) & 3);
    case THREE:
        return getBrush((mem[0xFF47] >> 6) & 3);
    }
}

QBrush GB_PPU::getBrush(unsigned char value) {
    switch (value) {
    case ZERO:
        return white;
    case ONE:
        return lightGray;
    case TWO:
        return darkGray;
    case THREE:
        return black;
    }
}
