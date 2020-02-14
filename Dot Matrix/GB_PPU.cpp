#include "GB_PPU.h"

GB_PPU::GB_PPU(unsigned char* cpuMem, QGraphicsScene* scene) {
	mem = cpuMem;
	display = scene;
	
	// create vram
	initVideo();
}

void GB_PPU::initVideo() {
	vram = new pixel * [256];
	for (int i = 0; i < 256; i++) {
		vram[i] = new pixel[256];
		for (int j = 0; j < 256; j++) {
			vram[i][j] = {THREE, 0, BACKGROUND};
		}
	}
}

// Renders display
// TODO: implement render function
void GB_PPU::render() {
	
}

// pixel first in first out
// TODO: implement pixelFIFO
void GB_PPU::pixelFIFO() {
	
}

// takes background map and copies
// corresponding tiles into video ram
void GB_PPU::setBackgroundTiles() {
    unsigned short bgDataAddr = BG_DATA_ADDR_0;
    unsigned short bgMapAddr = BG_WIN_MAP_ADDR_0;

    if (bgWinDataSelect() == 1) {
        bgDataAddr = BG_DATA_ADDR_1;
    }

    if (bgMapSelect() == 1) {
        bgMapAddr = BG_WIN_MAP_ADDR_1;
    }

    // iterate through tile map
    for (int index = 0; index < TILE_COUNT; index++) {
        unsigned short tileAddr = bgDataAddr + 16 * mem[bgMapAddr + index];
        int offsetX = (index % 32) * 8;
        int offsetY = (index / 32) * 8;

        // write tile to VRAM
        for (int row = 0; row < 8; row++) {
            unsigned char byte0 = mem[tileAddr + 2 * row];
            unsigned char byte1 = mem[tileAddr + 2 * row + 1];
            for (int col = 0; col < 8; col++) {

                // get pixel data for tile
                unsigned char bit1 = (byte0 >> (7 - col)) & 1;
                unsigned char bit0 = (byte1 >> (7 - col)) & 1;
                unsigned char color = (bit1 << 1) | bit0;
                vram[offsetY + row][offsetX + col] = {color, 0, BACKGROUND};
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

void GB_PPU::drawTileMap() {
	display->clear();
	
	QBrush zero = QBrush(QColor(255, 255, 255));
	QBrush one = QBrush(QColor(170, 170, 170));
	QBrush two = QBrush(QColor(85, 85, 85));
	QBrush three = QBrush(QColor(0, 0, 0));
	
	int offsetX = 0;
	int offsetY = 0;
	int x = 0;
	int y = 0;
	for (int i = 0; i < 0x1800; i += 2) {
		unsigned char byte1 = mem[i + 0x8000];
		unsigned char byte2 = mem[i + 0x8001];

		// set x offset for each tile col
		if (i % 16 == 0 && i != 0) {
			offsetX += 8;
			offsetX %= 160;
			offsetY = (i / (16 * 20)) * 8;
			y = 0;
		}

		for (int j = 0; j < 8; j++) {
			unsigned char value = ((byte1 >> (7 - j)) << 1) | (byte2 >> (7 - j));
			QBrush color;
			switch (value) {
			case ZERO:
				color = zero;
				break;
			case ONE:
				color = one;
				break;
			case TWO:
				color = two;
				break;
			case THREE:
				color = three;
				break;
			default:
				color = zero;
				break;
			}
			display->addRect(QRect(offsetX + x, offsetY + y, 1, 1), QPen(Qt::transparent), color);
			x++;
		}
		x = 0;
		y++;
	}
}
