#pragma once
#include "GB_CPU.h"
#include <QtWidgets/QMainWindow>
#include <mutex>

// pixel values
constexpr unsigned char ZERO = 0b00;
constexpr unsigned char ONE = 0b01;
constexpr unsigned char TWO = 0b10;
constexpr unsigned char THREE = 0b11;

// tile sizing
constexpr auto TILE_PX_DIM = 8;
constexpr auto BYTES_PER_TILE = 16;

// background map sizing
constexpr auto BG_TILE_DIM = 32;
constexpr auto BG_PX_DIM = BG_TILE_DIM * TILE_PX_DIM;
constexpr auto BG_TILE_COUNT = BG_TILE_DIM * BG_TILE_DIM;

// screen sizing
constexpr auto SCREEN_WIDTH = 160;
constexpr auto SCREEN_HEIGHT = 144;

// base addresses
constexpr auto BG_DATA_ADDR_0 = 0x8800;
constexpr auto BG_DATA_ADDR_1 = 0x8000;
constexpr auto BG_WIN_MAP_ADDR_0 = 0x9800;
constexpr auto BG_WIN_MAP_ADDR_1 = 0x9C00;

enum class PixelType {
	Background,
	Window,
	Sprite
};

struct pixel {
	unsigned char value;
	unsigned char palette;
	PixelType type;
};

class GB_PPU : public QWidget {
public:
	unsigned char* mem;
	pixel** vram;
	QBrush white, lightGray, darkGray, black;
	QMainWindow* dm;
	mutex* rw;
	
	GB_PPU(QMainWindow* dotMatrixClass);
	void initVideo();
	void render();
	void setBackgroundTiles();
	void setMemory(unsigned char* cpuMem);
	void setMutex(mutex* mutex);
	void paintEvent(QPaintEvent*);

    // lcd control
    int lcdDisplayEnable();
    int windowMapSelect();
    int windowDisplayEnable();
    int bgWinDataSelect();
    int bgMapSelect();
    int spriteSize();
    int spriteEnable();
    int bgDisplayEnable();

	// background palette data
	QBrush getShade(unsigned char value);
	QBrush getBrush(unsigned char value);
};
