#pragma once

#include "memory.h"
#include "utils.h"

class PPU : public QThread {
  Q_OBJECT

 private:
  uint8 &lcdc, &stat, &scy, &scx, &ly, &lyc, &dma, &bgp, &obp0, &obp1, &wy, &wx;
  sprite_t visibleSprites[MAX_SPRITES_PER_LINE];
  uint8 visibleSpriteCount;
  float &speedMult;
  Memory &mem;
  Palette *palette;
  bool running;

  // rendering functions
  void renderBg();
  void renderSprites();
  void renderWindow();
  void findVisibleSprites();
  void applyPalettes();

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

 public:
  QImage screen;

  PPU(Memory &mem, Palette *palette, float &speedMult);
  ~PPU();

  void run() override;

 signals:
  void sendScreen(QImage *);
};
