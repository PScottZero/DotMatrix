#pragma once
#include "GB_CPU.h"
#include "DotMatrix.h"
#include <QtWidgets/QApplication>

constexpr auto ZERO = 0b00;
constexpr auto ONE = 0b01;
constexpr auto TWO = 0b10;
constexpr auto THREE = 0b11;
constexpr auto TILE_COUNT = 32 * 32;

// addresses
constexpr auto BG_DATA_ADDR_0 = 0x8800;
constexpr auto BG_DATA_ADDR_1 = 0x8000;
constexpr auto BG_WIN_MAP_ADDR_0 = 0x9800;
constexpr auto BG_WIN_MAP_ADDR_1 = 0x9C00;

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

    // lcd control
    int lcdDisplayEnable();
    int windowMapSelect();
    int windowDisplayEnable();
    int bgWinDataSelect();
    int bgMapSelect();
    int spriteSize();
    int spriteEnable();
    int bgDisplayEnable();
};
