#ifndef PPU_H
#define PPU_H


#include "cpu.h"
#include <QtWidgets/QMainWindow>

// pixel values
constexpr unsigned char PX_ZERO = 0b00;
constexpr unsigned char PX_ONE = 0b01;
constexpr unsigned char PX_TWO = 0b10;
constexpr unsigned char PX_THREE = 0b11;

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

class PPU : public QWidget {
public:
    unsigned char* mem;
    pixel** vram;
    QBrush white, lightGray, darkGray, black;
    QMainWindow* dm;

    PPU(QMainWindow* dotMatrixClass);
    void initVideo();
    void render();
    void setBackgroundTiles();
    void setMemory(unsigned char* cpuMem);
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


#endif // PPU_H
