#pragma once

#include <QImage>
#include <QThread>
#include <array>
#include <thread>

#include "memory.h"
#include "palette.h"
#include "types.h"
#include "interrupts.h"
#include "clock.h"

// clock speed constants
#define PPU_CLOCK_SPEED 0x100000
#define OAM_SEARCH_CYCLES 20
#define PIXEL_TRANSFER_CYCLES 43
#define H_BLANK_CYCLES 51
#define V_BLANK_CYCLES 114
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

// sprite oam entry
typedef struct {
  uint8 y;
  uint8 x;
  uint8 pattern;
  bool priority;
  bool flipY;
  bool flipX;
  bool palette;
} sprite_t;

enum PaletteType {
  BG,
  SPRITE0,
  SPRITE1
};

typedef struct {
  uint8 pixels[SCREEN_PX_WIDTH];
  PaletteType palettes[SCREEN_PX_WIDTH];
  uint8 spriteIndices[SCREEN_PX_WIDTH];
} scanline_t;

// tile row of eight pixels
using TileRow = std::array<uint8, TILE_PX_DIM>;

class PPU : public QThread {
  Q_OBJECT

 private:
  uint8 &lcdc, &stat, &scy, &scx, &ly, &lyc, &dma, &bgp, &obp0, &obp1, &wy, &wx;
  sprite_t visibleSprites[MAX_SPRITES_PER_LINE];
  uint8 visibleSpriteCount;
  float &speedMult;
  Memory &mem;
  Clock clock;
  Palette *palette;
  bool &stop, &threadRunning, &bootstrapMode;

  // rendering functions
  void renderBg(scanline_t &scanline);
  void renderSprites(scanline_t &scanline);
  void renderWindow(scanline_t &scanline);
  void findVisibleSprites();
  void transferScanlineToScreen(scanline_t &scanline);

  // read display memory functions
  TileRow getTileRow(uint16 baseAddr, uint8 tileNo, uint8 row);
  TileRow getSpriteRow(sprite_t oamEntry, uint8 row);
  sprite_t getSpriteOAM(uint8 spriteIdx);

  // lcdc register functions
  bool lcdEnable();
  bool bgEnable();
  bool windowEnable();
  bool spriteEnable();
  uint8 spriteHeight();
  uint16 windowMapAddr();
  uint16 bgWindowDataAddr();
  uint16 bgMapAddr();

  // stat register functions
  void setMode(uint8 mode);
  void setLCDInterrupt();

 public:
  QImage screen;

  PPU(Memory &mem, Palette *palette, float &speedMult, bool &stop,
      bool &threadRunning, bool &bootstrapMode);
  ~PPU();

  void run() override;

 signals:
  void sendScreen(QImage);
};
