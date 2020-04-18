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
// PPU addresses
// ================================
constexpr auto DATA_ADDR_0 = 0x8800;
constexpr auto DATA_ADDR_1 = 0x8000;
constexpr auto MAP_ADDR_0 = 0x9800;
constexpr auto MAP_ADDR_1 = 0x9C00;
constexpr auto LCDC = 0xFF40;
constexpr auto LCD_STAT = 0xFF41;
constexpr auto SCROLL_Y = 0xFF42;
constexpr auto SCROLL_X = 0xFF43;
constexpr auto LY = 0xFF44;
constexpr auto LY_COMP = 0xFF45;
constexpr auto BGP = 0xFF47;
constexpr auto OBP0 = 0xFF48;
constexpr auto OBP1 = 0xFF49;
constexpr auto WINDOW_Y = 0xFF4A;
constexpr auto WINDOW_X = 0xFF4B;

// ================================
// Pixel type
// ================================
enum PixelType {
    BACKGROUND, WINDOW, SPRITE
};

// ================================
// Screen timing
// ================================
constexpr auto SCANLINE = 114;
constexpr auto SCREEN_CYCLE = 17556;

// ================================
// Display modes
// ================================
enum Mode {
    MODE_0 = 0b00,
    MODE_1 = 0b01,
    MODE_2 = 0b10,
    MODE_3 = 0b11,
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
    QImage *display;
    bool rendered;
    unsigned int ppuCycle;
    unsigned int prevClock;
    unsigned int currClock;

    // ================================
    // Display functions
    // ================================
    PPU(unsigned char *cpuMem, unsigned int *cpuClock, QImage *frame);
    void step();
    void render();
    uint getPixelColor(unsigned char value, unsigned short mapAddr) const;
    void setMode(Mode m) const;

    // ================================
    // LCD control functions
    // ================================
    int lcdDisplayEnable() const;
    int windowMapSelect() const;
    int windowDisplayEnable() const;
    int bgWinDataSelect() const;
    int bgMapSelect() const;
    int spriteSize() const;
    int spriteEnable() const;
    int bgDisplayEnable() const;

    // ================================
    // OAM functions
    // ================================
    unsigned char getSpriteY(int oamAddr) const;
    unsigned char getSpriteX(int oamAddr) const;
    unsigned char getSpriteTileNo(int oamEntry) const;
    bool spriteBehindBG(int oamEntry) const;
    bool spriteFlipY(int oamEntry) const;
    bool spriteFlipX(int oamEntry) const;
    bool getSpritePalette(int oamEntry) const;

    // ================================
    // Interrupt functions
    // ================================
    void triggerVBlankInt() const;
    void setLcdInt() const;
};

#endif // PPU_H
