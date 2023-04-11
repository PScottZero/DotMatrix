#include "ppu.h"

#include <stdlib.h>

PPU::PPU(Memory &mem, int &speedMult)
    : screen(SCREEN_PX_WIDTH, SCREEN_PX_HEIGHT, QImage::Format_RGB32),
      lcdc(mem.getRef(LCDC)),
      stat(mem.getRef(STAT)),
      scy(mem.getRef(SCY)),
      scx(mem.getRef(SCX)),
      ly(mem.getRef(LY)),
      lyc(mem.getRef(LYC)),
      dma(mem.getRef(DMA)),
      bgp(mem.getRef(BGP)),
      obp0(mem.getRef(OBP0)),
      obp1(mem.getRef(OBP1)),
      wy(mem.getRef(WY)),
      wx(mem.getRef(WX)),
      visibleSprites(),
      visibleSpriteCount(),
      speedMult(speedMult),
      mem(mem) {}

void PPU::run() {
  while (true) {
    if (lcdEnable()) {
      for (uint8 lineNo = 0; lineNo < SCREEN_LINES; ++lineNo) {
        int cycleTimeNs = NS_PER_SEC / (PPU_CLOCK_SPEED * speedMult);
        ly = lineNo;

        // check if current line number
        // is equal to the value in LYC
        if (ly == lyc) {
          stat |= 0x04;
        } else {
          stat &= 0xFB;
        }

        if (lineNo < SCREEN_PX_HEIGHT) {
          // horizontal timing
          auto start = chrono::system_clock::now();
          auto oamSearchEnd =
              start + chrono::nanoseconds(OAM_SEARCH_CYCLES * cycleTimeNs);
          auto pixelTransferEnd =
              oamSearchEnd +
              chrono::nanoseconds(PIXEL_TRANSFER_CYCLES * cycleTimeNs);
          auto hblankEnd = pixelTransferEnd +
                           chrono::nanoseconds(H_BLANK_CYCLES * cycleTimeNs);

          // ==================================================
          // OAM Search
          // ==================================================
          setMode(OAM_SEARCH_MODE);
          findVisibleSprites();
          this_thread::sleep_until(oamSearchEnd);

          // ==================================================
          // Pixel Transfer
          // ==================================================
          setMode(PIXEL_TRANSFER_MODE);
          if (bgEnable()) renderBg();
          if (spriteEnable()) renderSprites();
          if (windowEnable()) renderWindow();
          applyPalettes();
          this_thread::sleep_until(pixelTransferEnd);

          // ==================================================
          // H-Blank
          // ==================================================
          setMode(H_BLANK_MODE);
          this_thread::sleep_until(hblankEnd);
        } else {
          // ==================================================
          // V-Blank
          // ==================================================
          setMode(V_BLANK_MODE);
          auto vblankStart = chrono::system_clock::now();
          auto vblankEnd =
              vblankStart + chrono::nanoseconds(V_BLANK_CYCLES * cycleTimeNs);
          this_thread::sleep_until(vblankEnd);
        }
      }
    }
  }
}

// **************************************************
// **************************************************
// Rendering Functions
// **************************************************
// **************************************************

// render background tiles
void PPU::renderBg() {
  uint16 tileMapAddr = bgMapAddr();

  // render bg row
  int pxY = (scy + ly) % BG_PX_DIM;
  int pxCount = 0;
  while (pxCount < SCREEN_PX_WIDTH) {
    int pxX = (scx + pxCount) % BG_PX_DIM;
    int tileX = pxX / BG_TILE_DIM;
    int tileY = pxY / BG_TILE_DIM;
    int tileNo = tileY * BG_TILE_DIM + tileX;
    int innerTileX = pxX % TILE_PX_DIM;
    int innerTileY = pxY % TILE_PX_DIM;
    uint8 tileMap = mem.getByte(tileMapAddr + tileNo);

    // draw tile row onto screen
    TileRow row = getTileRow(tileMap, innerTileY);
    for (int px = 0; px < TILE_PX_DIM - innerTileX; ++px) {
      if (pxCount < SCREEN_PX_WIDTH) {
        screen.setPixel(pxCount, ly, row[px + innerTileX]);
      } else {
        break;
      }
      ++pxCount;
    }
  }
}

// render sprites that intersect the screen
// row that is being rendered
void PPU::renderSprites() {
  for (int spriteIdx = 0; spriteIdx < visibleSpriteCount; ++spriteIdx) {
    sprite_t sprite = visibleSprites[spriteIdx];
    uint8 spriteRow = ly - sprite.y - 16;
    TileRow row = getSpriteRow(sprite, spriteRow);

    // draw sprite row onto screen
    for (int rowIdx = 0; rowIdx < TILE_PX_DIM; ++rowIdx) {
      int pxX = sprite.x + rowIdx - 8;
      if (pxX >= 0 && pxX < SCREEN_PX_WIDTH) {
        if (!sprite.priority || screen.pixel(pxX, ly) == 0) {
          uint32 writeVal = ((sprite.palette ? 0b10 : 0b01) << 2) | row[rowIdx];
          screen.setPixel(pxX, ly, writeVal);
        }
      }
    }
  }
}

// render window
void PPU::renderWindow() {}

// find which sprites intersect the
// current scanline
void PPU::findVisibleSprites() {
  visibleSpriteCount = 0;
  for (uint8 oamIdx = 0; oamIdx < OAM_ENTRY_COUNT; oamIdx++) {
    sprite_t oamEntry = getSpriteOAM(oamIdx);
    if (oamEntry.y >= ly + SPRITE_PX_HEIGHT_TALL &&
        oamEntry.y < ly + SPRITE_PX_HEIGHT_TALL + spriteHeight() &&
        oamEntry.x != 0) {
      visibleSprites[visibleSpriteCount] = oamEntry;
      ++visibleSpriteCount;

      // stop once we have found 10 sprites
      if (visibleSpriteCount == MAX_SPRITES_PER_LINE) {
        break;
      }
    }
  }
}

// apply palette colors to current pixel
// values in the screen buffer
void PPU::applyPalettes() {
  for (int x = 0; x < SCREEN_PX_WIDTH; ++x) {
    uint32 px = screen.pixel(x, ly);
    uint8 pxVal = px & 0b11;
    uint8 palOption = px >> 2 & 0b11;
    uint8 palette = 0;

    switch (palOption) {
      case 0b00:
        palette = bgp;
        break;
      case 0b01:
        palette = obp0;
        break;
      case 0b10:
        palette = obp1;
        break;
    }

    uint pxPalVal = (palette >> (2 * pxVal)) & 0b11;
    screen.setPixel(x, ly, defaultPal[pxPalVal]);
  }
}

// **************************************************
// **************************************************
// Read Display Memory Functions
// **************************************************
// **************************************************

// get specified row of given tile
TileRow PPU::getTileRow(uint8 tileNo, uint8 row) {
  // get pointer to tile data
  uint16 tileDataAddr = bgDataAddr();
  uint16 tileAddr = tileDataAddr * tileNo * TILE_BYTES;
  uint16 *tile = (uint16 *)mem.getPtr(tileAddr);

  // split row data into hi and lo byte
  uint16 rowData = *(tile + row);
  uint8 rowDataHi = (rowData >> 8) & 0xFF;
  uint8 rowDataLo = rowData & 0xFF;

  // row (uint16) = abcdefgh ijklmnop
  // pxData (uint8[]) = ai bj ck dl em fn go hp
  TileRow pxRow;
  for (int i = 0; i < TILE_PX_DIM; i++) {
    uint8 pxIdx = TILE_PX_DIM - i - 1;
    pxRow[i] = (rowDataHi >> (pxIdx - 1)) & 0b10;
    pxRow[i] |= (rowDataLo >> pxIdx) & 0b1;
  }
  return pxRow;
}

// get specified row of given sprite
TileRow PPU::getSpriteRow(sprite_t oamEntry, uint8 row) {
  uint8 height = spriteHeight();
  row = oamEntry.flipY ? height - row - 1 : row;
  TileRow pxRow = getTileRow(oamEntry.pattern, row);
  if (oamEntry.flipX) {
    for (uint8 i = 0; i < pxRow.size() / 2; ++i) {
      uint8 swapIdx = pxRow.size() - i - 1;
      uint8 temp = pxRow[swapIdx];
      pxRow[swapIdx] = pxRow[i];
      pxRow[i] = temp;
    }
  }
  return pxRow;
}

// get sprite oam entry at a given sprite index
sprite_t PPU::getSpriteOAM(uint8 spriteIdx) {
  sprite_t sprite;
  uint16 spriteAddr = OAM_ADDR + spriteIdx * OAM_ENTRY_BYTES;

  // get sprite position and pattern
  sprite.y = mem.getByte(spriteAddr);
  sprite.x = mem.getByte(spriteAddr + 1);
  sprite.pattern = mem.getByte(spriteAddr + 2);

  // get sprite flags
  uint8 flags = mem.getByte(spriteAddr + 3);
  sprite.priority = flags & 0x80;
  sprite.flipY = flags & 0x40;
  sprite.flipX = flags & 0x20;
  sprite.palette = flags & 0x10;

  return sprite;
}

// **************************************************
// **************************************************
// LCDC Register Functions
// **************************************************
// **************************************************

bool PPU::lcdEnable() { return lcdc & 0x80; }

bool PPU::bgEnable() { return lcdc & 0x01; }

bool PPU::windowEnable() { return lcdc & 0x20; }

bool PPU::spriteEnable() { return lcdc & 0x02; }

uint8 PPU::spriteHeight() {
  return lcdc & 0x04 ? SPRITE_PX_HEIGHT_TALL : SPRITE_PX_HEIGHT_SHORT;
}

uint16 PPU::windowMapAddr() {
  return lcdc & 0x40 ? WINDOW_CODE_ADDR_1 : WINDOW_CODE_ADDR_0;
}

uint16 PPU::bgDataAddr() {
  return lcdc & 0x10 ? BG_DATA_ADDR_1 : BG_DATA_ADDR_0;
}

uint16 PPU::bgMapAddr() {
  return lcdc & 0x08 ? BG_CODE_ADDR_1 : BG_CODE_ADDR_0;
}

// **************************************************
// **************************************************
// STAT Register Functions
// **************************************************
// **************************************************

void PPU::setMode(uint8 mode) {
  stat &= 0xFC;
  stat |= mode;
}
