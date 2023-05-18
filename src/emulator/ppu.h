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

class CGB;

class PPU : public QObject {
  Q_OBJECT

 private:
  uint8 windowLineNum;
  sprite_t visibleSprites[MAX_SPRITES_PER_LINE];
  uint8 visibleSpriteCount;
  bool statInt;

  // OAM search functions
  void findVisibleSprites();

  // rendering functions
  void renderBg(scanline_t &scanline);
  void renderWindow(scanline_t &scanline);
  void renderSprites(scanline_t &scanline);
  bool spriteHasPriority(sprite_t &sprite, scanline_t &scanline,
                         uint8 scanlineIdx, uint8 px);
  void transferScanlineToScreen(scanline_t &scanline);
  void resetScanline(scanline_t &scanline);

  // read display memory functions
  TileRow getTileRow(uint16 baseAddr, uint8 tileNo, uint8 row,
                     bool vramBank = false) const;
  TileRow getTileRow(tile_map_attr_t bgMapAttr, uint8 tileNo, uint8 row) const;
  TileRow getSpriteRow(sprite_t oamEntry, uint8 row) const;
  sprite_t getSpriteOAM(uint8 spriteIdx) const;
  tile_map_attr_t getTileMapAttr(uint16 baseAddr, uint16 bgWinTileNo) const;
  void flipTileRow(TileRow &row) const;

  // lcdc register functions
  bool lcdEnable() const;
  bool bgEnable() const;
  bool windowEnable() const;
  bool spriteEnable() const;
  uint8 spriteHeight() const;
  uint16 windowMapAddr() const;
  uint16 bgWindowDataAddr() const;
  uint16 bgMapAddr() const;

  // stat register functions
  bool coincidenceIntEnabled() const;
  bool oamSearchIntEnabled() const;
  bool vblankIntEnabled() const;
  bool hblankIntEnabled() const;
  bool lyEqualsLyc() const;
  void setMode(uint8 mode);
  uint8 getMode() const;
  void setLcdStatInterrupt();

  // lcd color palette functions
  uint getPaletteColor(uint8 *cram, uint8 palIdx, uint8 colorIdx) const;

 public:
  uint8 *lcdc, *stat, *scy, *scx, *ly, *lyc, *bgp, *obp0, *obp1, *wy, *wx;

  CGB *cgb;
  QImage *screen;
  bool frameRendered;
  Palette *palette;
  bool showBackground, showWindow, showSprites;
  uint16 cycles;

  PPU();

  void step();

 public slots:
  void toggleBackground(bool show);
  void toggleWindow(bool show);
  void toggleSprites(bool show);
};
