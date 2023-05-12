// **************************************************
// **************************************************
// **************************************************
// PIXEL PROCESSING UNIT (PPU)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QImage>
#include <array>
#include <thread>

#include "../ui/palettes.h"
#include "types.h"

// clock speed constants
#define PPU_CLOCK_SPEED 0x100000
#define OAM_SEARCH_CYCLES 20
#define PIXEL_TRANSFER_CYCLES OAM_SEARCH_CYCLES + 43
#define H_BLANK_CYCLES PIXEL_TRANSFER_CYCLES + 51
#define V_BLANK_CYCLES 114
#define SCANLINE_CYCLES 114
#define _NS_PER_SEC 1000000000

// px constants
#define BG_PX_DIM 256
#define BG_TILE_DIM 32
#define TILE_PX_DIM 8
#define SCREEN_PX_WIDTH 160
#define SCREEN_PX_HEIGHT 144
#define SCREEN_LINES 154
#define SPRITE_PX_HEIGHT_SHORT 8
#define SPRITE_PX_HEIGHT_TALL 16

// byte constants
#define VIEW_BUF_BYTES 160 * 144
#define TILE_BYTES 16
#define OAM_ENTRY_BYTES 4

// count constants
#define BG_TILE_COUNT 32 * 32
#define OAM_ENTRY_COUNT 40
#define MAX_SPRITES_PER_LINE 10

// screen modes
#define H_BLANK_MODE 0b00
#define V_BLANK_MODE 0b01
#define OAM_SEARCH_MODE 0b10
#define PIXEL_TRANSFER_MODE 0b11

// screen memory address constants
#define WINDOW_MAP_ADDR_0 0x9800
#define WINDOW_MAP_ADDR_1 0x9C00
#define BG_DATA_ADDR_0 0x9000
#define BG_DATA_ADDR_1 0x8000
#define BG_MAP_ADDR_0 0x9800
#define BG_MAP_ADDR_1 0x9C00

using namespace std;

// sprite oam entry
typedef struct {
  uint8 y;
  uint8 x;
  uint8 pattern;
  bool priority;
  bool flipY;
  bool flipX;
  bool palette;
  bool vramBankNum;
  uint8 paletteNum;
} sprite_t;

// background map attributes,
// one per background tile in
// cgb mode
typedef struct {
  bool priority;
  bool flipY;
  bool flipX;
  bool vramBankNum;
  uint8 paletteNum;
} tile_map_attr_t;

enum PaletteType { BG, SPRITE0, SPRITE1 };

typedef struct {
  uint8 pixels[SCREEN_PX_WIDTH];
  PaletteType paletteTypes[SCREEN_PX_WIDTH];
  uint8 spriteIndices[SCREEN_PX_WIDTH];
  uint8 paletteIndices[SCREEN_PX_WIDTH];  // cgb only
  bool priorities[SCREEN_PX_WIDTH];       // cgb only
} scanline_t;

// tile row of eight pixels
using TileRow = array<uint8, TILE_PX_DIM>;

class PPU {
 private:
  static uint8 &lcdc, &stat, &scy, &scx, &ly, &lyc, &dma, &bgp, &obp0, &obp1,
      &wy, &wx, &bcps, &bcpd, &ocps, &ocpd, windowLineNum;
  static sprite_t visibleSprites[MAX_SPRITES_PER_LINE];
  static uint8 visibleSpriteCount;
  static bool statInt;

  // OAM search functions
  static void findVisibleSprites();

  // rendering functions
  static void renderBg(scanline_t &scanline);
  static void renderWindow(scanline_t &scanline);
  static void renderSprites(scanline_t &scanline);
  static bool spriteHasPriority(sprite_t &sprite, scanline_t &scanline,
                                uint8 scanlineIdx, uint8 px);
  static void transferScanlineToScreen(scanline_t &scanline);
  static void resetScanline(scanline_t &scanline);

  // read display memory functions
  static TileRow getTileRow(uint16 baseAddr, uint8 tileNo, uint8 row,
                            bool vramBank = false);
  static TileRow getTileRow(tile_map_attr_t bgMapAttr, uint8 tileNo, uint8 row);
  static TileRow getSpriteRow(sprite_t oamEntry, uint8 row);
  static sprite_t getSpriteOAM(uint8 spriteIdx);
  static tile_map_attr_t getTileMapAttr(uint16 baseAddr, uint8 tileNo);
  static void flipTileRow(TileRow &row);

  // lcdc register functions
  static bool lcdEnable();
  static bool bgEnable();
  static bool windowEnable();
  static bool spriteEnable();
  static uint8 spriteHeight();
  static uint16 windowMapAddr();
  static uint16 bgWindowDataAddr();
  static uint16 bgMapAddr();

  // stat register functions
  static bool coincidenceIntEnabled();
  static bool oamSearchIntEnabled();
  static bool vblankIntEnabled();
  static bool hblankIntEnabled();
  static bool lyEqualsLyc();
  static void setMode(uint8 mode);
  static uint8 getMode();
  static void setLcdStatInterrupt();

  // lcd color palette functions
  static uint getPaletteColor(uint8 *cram, uint8 palIdx, uint8 colorIdx);

 public:
  static QImage *screen;
  static bool frameRendered;
  static Palette *palette;
  static bool showBackground, showWindow, showSprites;

  static void step();
};
