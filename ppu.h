#ifndef PPU_H
#define PPU_H

#include "cpu.h"
#include <QtWidgets/QMainWindow>

// ================================
// Pixel color values
// ================================
constexpr unsigned char PX_ZERO = 0b00;
constexpr unsigned char PX_ONE = 0b01;
constexpr unsigned char PX_TWO = 0b10;
constexpr unsigned char PX_THREE = 0b11;

// ================================
// Tile sizing
// ================================
constexpr auto TILE_PX_DIM = 8;
constexpr auto BYTES_PER_TILE = 16;

// ================================
// Background map sizing
// ================================
constexpr auto BG_TILE_DIM = 32;
constexpr auto BG_PX_DIM = BG_TILE_DIM * TILE_PX_DIM;
constexpr auto BG_TILE_COUNT = BG_TILE_DIM * BG_TILE_DIM;

// ================================
// Screen sizing
// ================================
constexpr auto SCREEN_WIDTH = 160;
constexpr auto SCREEN_HEIGHT = 144;

// ================================
// Base addresses
// ================================
constexpr auto BG_DATA_ADDR_0 = 0x8800;
constexpr auto BG_DATA_ADDR_1 = 0x8000;
constexpr auto BG_WIN_MAP_ADDR_0 = 0x9800;
constexpr auto BG_WIN_MAP_ADDR_1 = 0x9C00;

// ================================
// Screen timing
// ================================
constexpr auto SCANLINE = 114;
constexpr auto SCREEN_CYCLE = 17556;

// ================================
// Display register addresses
// ================================
constexpr unsigned short SCROLL_Y = 0xFF42;
constexpr unsigned short SCROLL_X = 0xFF43;
constexpr unsigned short LCDC_Y = 0xFF44;
constexpr unsigned short LY_COMP = 0xFF45;
constexpr unsigned short WINDOW_Y = 0xFF4A;
constexpr unsigned short WINDOW_X = 0xFF4B;

// ================================
// Pixel type
// ================================
enum class PixelType {
    Background,
    Window,
    Sprite
};

// ================================
// Display modes
// ================================
enum class Mode {
    MODE_0,
    MODE_1,
    MODE_2,
    MODE_3,
};

// ================================
// Pixel structure
// ================================
struct pixel {
    unsigned char value;
    unsigned char palette;
    PixelType type;
};

class PPU {
public:

    // ================================
    // Instance data
    // ================================
    unsigned char* mem;
    unsigned int* clk;
    Mode ppuMode;
    QImage *display;
    int currLine;

    // ================================
    // Display functions
    // ================================
    PPU(unsigned char *cpuMem, unsigned int *cpuClock, QImage *frame);
    void step();
    void render();

    // ================================
    // LCD control functions
    // ================================
    int lcdDisplayEnable();
    int windowMapSelect();
    int windowDisplayEnable();
    int bgWinDataSelect();
    int bgMapSelect();
    int spriteSize();
    int spriteEnable();
    int bgDisplayEnable();

    // ================================
    // Background palette functions
    // ================================
    uint getPixelColor(unsigned char value);
    uint getColor(unsigned char value);
};

#endif // PPU_H
