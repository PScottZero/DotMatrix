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

void GB_PPU::setBackgroundTiles() {
	for (int i = 0; i < TILE_COUNT; i++) {
		unsigned char tileNo = mem[0x9800 + i];
		for (int row = 0; row < 8; row++) {
			unsigned char byte0 = mem[0x8000 + ]
		}
	}
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
