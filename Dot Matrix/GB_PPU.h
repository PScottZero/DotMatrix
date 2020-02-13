#pragma once
#include "GB_CPU.h"
#include "DotMatrix.h"
#include <QtWidgets/QApplication>

constexpr auto ZERO = 0b00;
constexpr auto ONE = 0b01;
constexpr auto TWO = 0b10;
constexpr auto THREE = 0b11;
constexpr auto TILE_COUNT = 32 * 32;

enum PixelType {
	BACKGROUND,
	WINDOW,
	SPRITE
};

struct pixel {
	unsigned char value;
	unsigned char palette;
	PixelType type;
};

struct pixelFifo {
	unsigned char size;
	pixel fifo[16];
};

class GB_PPU {
public:
	unsigned char* mem;
	pixel** vram;
	QGraphicsScene* display;
	GB_PPU(unsigned char* cpuMem, QGraphicsScene* scene);
	void initVideo();
	void render();
	void pixelFIFO();
	void setBackgroundTiles();
	void drawTileMap();
};
