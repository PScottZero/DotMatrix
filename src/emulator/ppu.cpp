// **************************************************
// **************************************************
// **************************************************
// Pixel Processing Unit (PPU)
// **************************************************
// **************************************************
// **************************************************

#include "ppu.h"

#include "bootstrap.h"
#include "cyclecounter.h"
#include "interrupts.h"
#include "memory.h"

QImage *PPU::screen = nullptr;

// initialize references to in-memory
// registers used by the PPU
uint8 &PPU::lcdc = Memory::getByte(LCDC);
uint8 &PPU::stat = Memory::getByte(STAT);
uint8 &PPU::scy = Memory::getByte(SCY);
uint8 &PPU::scx = Memory::getByte(SCX);
uint8 &PPU::ly = Memory::getByte(LY);
uint8 &PPU::lyc = Memory::getByte(LYC);
uint8 &PPU::dma = Memory::getByte(DMA);
uint8 &PPU::bgp = Memory::getByte(BGP);
uint8 &PPU::obp0 = Memory::getByte(OBP0);
uint8 &PPU::obp1 = Memory::getByte(OBP1);
uint8 &PPU::wx = Memory::getByte(WX);
uint8 &PPU::wy = Memory::getByte(WY);

// initialize visible sprite
// array and count
sprite_t PPU::visibleSprites[MAX_SPRITES_PER_LINE]{};
uint8 PPU::visibleSpriteCount = 0;

// initialize palette
Palette *PPU::palette = &Palettes::palGBP;

// initialize frame rendered flag
bool PPU::frameRendered = false;

void PPU::step() {
  if (lcdEnable()) {
    if (CycleCounter::ppuCycles > SCANLINE_CYCLES) {
      if (++ly >= SCREEN_LINES) ly = 0;

      // check if current line number
      // is equal to the value in LYC
      if (ly == lyc) {
        stat |= 0x04;
        setLCDInterrupt();
      } else {
        stat &= 0xFB;
      }

      CycleCounter::ppuCycles %= SCANLINE_CYCLES;
    }

    if (ly < SCREEN_PX_HEIGHT) {
      // **************************************************
      // OAM Search
      // **************************************************
      if (CycleCounter::ppuCycles < OAM_SEARCH_CYCLES) {
        if (getMode() != OAM_SEARCH_MODE) {
          setMode(OAM_SEARCH_MODE);
          setLCDInterrupt();
          if (!Bootstrap::skipWait()) {
            findVisibleSprites();
          }
        }
      }

      // **************************************************
      // Pixel Transfer
      // **************************************************
      else if (CycleCounter::ppuCycles < PIXEL_TRANSFER_CYCLES) {
        if (getMode() != PIXEL_TRANSFER_MODE) {
          setMode(PIXEL_TRANSFER_MODE);
          setLCDInterrupt();
          if (!Bootstrap::skipWait()) {
            scanline_t scanline;
            if (bgEnable()) renderBg(scanline);
            if (spriteEnable()) renderSprites(scanline);
            if (windowEnable()) renderWindow(scanline);
            transferScanlineToScreen(scanline);
          }
        }
      }

      // **************************************************
      // H-Blank
      // **************************************************
      else if (CycleCounter::ppuCycles < H_BLANK_CYCLES) {
        if (getMode() != H_BLANK_MODE) {
          setMode(H_BLANK_MODE);
          setLCDInterrupt();
        }
      }
    } else {
      // **************************************************
      // V-Blank
      // **************************************************
      if (getMode() != V_BLANK_MODE) {
        setMode(V_BLANK_MODE);
        setLCDInterrupt();
        Interrupts::request(V_BLANK_INT);
        frameRendered = true;
      }
    }
  } else {
    ly = 0;
    setMode(H_BLANK_MODE);
    screen->fill(palette->data[0]);
    CycleCounter::ppuCycles = 0;
  }
}

// **************************************************
// **************************************************
// Rendering Functions
// **************************************************
// **************************************************

// render background tiles that intersect
// the current scanline
void PPU::renderBg(scanline_t &scanline) {
  uint16 tileMapAddr = bgMapAddr();
  uint16 tileDataAddr = bgWindowDataAddr();

  // render background row
  int pxY = (scy + ly) % BG_PX_DIM;
  int pxCount = 0;
  while (pxCount < SCREEN_PX_WIDTH) {
    // get background tile number (0 to 1023)
    int pxX = (scx + pxCount) % BG_PX_DIM;
    int bgTileX = pxX / TILE_PX_DIM;
    int bgTileY = pxY / TILE_PX_DIM;
    int innerBgTileX = pxX % TILE_PX_DIM;
    int innerBgTileY = pxY % TILE_PX_DIM;
    int bgTileNo = bgTileY * BG_TILE_DIM + bgTileX;

    // get data tile number (0 to 256 or -128 to 127)
    int tileNo = Memory::getByte(tileMapAddr + bgTileNo);

    // get row of pixels from tile
    TileRow row = getTileRow(tileDataAddr, tileNo, innerBgTileY);

    // transfer pixel row to scanline
    int oldPxCount = pxCount;
    int start = pxCount == 0 ? innerBgTileX : 0;
    int end = pxCount + 8 > 160 ? innerBgTileX + 1 : TILE_PX_DIM;
    for (int i = start; i < end; ++i) {
      scanline.pixels[pxCount] = row[i];
      scanline.palettes[pxCount] = PaletteType::BG;
      ++pxCount;
    }

    if (pxCount == oldPxCount) {
      break;
    }
  }
}

// render sprites that intersect the
// current scanline
void PPU::renderSprites(scanline_t &scanline) {
  for (int spriteIdx = 0; spriteIdx < visibleSpriteCount; ++spriteIdx) {
    sprite_t sprite = visibleSprites[spriteIdx];
    uint8 spriteRow = (ly + 16) - sprite.y;
    TileRow row = getSpriteRow(sprite, spriteRow);

    // draw sprite row onto screen
    for (int rowIdx = 0; rowIdx < TILE_PX_DIM; ++rowIdx) {
      int pxX = sprite.x + rowIdx - 8;
      if (pxX >= 0 && pxX < SCREEN_PX_WIDTH) {
        // if sprite priority is true, then only draw sprite
        // if current scaline color is zero
        if ((!sprite.priority || scanline.pixels[pxX] == 0) &&
            row[rowIdx] != 0) {
          scanline.pixels[pxX] = row[rowIdx];
          scanline.palettes[pxX] =
              sprite.palette ? PaletteType::SPRITE1 : PaletteType::SPRITE0;
          scanline.spriteIndices[pxX] = spriteIdx;
        }
      }
    }
  }
}

// render window
void PPU::renderWindow(scanline_t &scanline) {
  // uint16 tileMapAddr = windowMapAddr();
  // uint16 tileDataAddr = bgWindowDataAddr();

  // for (int i = 0; i < BG_TILE_DIM; i++) {

  // }
}

// find which sprites intersect the
// current scanline
void PPU::findVisibleSprites() {
  visibleSpriteCount = 0;
  for (uint8 oamIdx = 0; oamIdx < OAM_ENTRY_COUNT; oamIdx++) {
    sprite_t oamEntry = getSpriteOAM(oamIdx);
    if ((ly + SPRITE_PX_HEIGHT_TALL) >= oamEntry.y &&
        (ly + SPRITE_PX_HEIGHT_TALL) < (oamEntry.y + spriteHeight()) &&
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
void PPU::transferScanlineToScreen(scanline_t &scanline) {
  for (int px = 0; px < SCREEN_PX_WIDTH; ++px) {
    uint8 pal;
    switch (scanline.palettes[px]) {
      case PaletteType::BG:
        pal = bgp;
        break;
      case PaletteType::SPRITE0:
        pal = obp0;
        break;
      case PaletteType::SPRITE1:
        pal = obp1;
        break;
    }
    uint8 pxPalVal = (pal >> (2 * scanline.pixels[px])) & 0b11;
    screen->setPixel(px, ly, palette->data[pxPalVal]);
  }
}

// **************************************************
// **************************************************
// Read Display Memory Functions
// **************************************************
// **************************************************

// get specified row of the given tile
TileRow PPU::getTileRow(uint16 baseAddr, uint8 tileNo, uint8 row) {
  // get tile row data
  int16 tileNoSigned = baseAddr == BG_DATA_ADDR_0 ? (int8)tileNo : tileNo;
  uint16 tileAddr = baseAddr + tileNoSigned * TILE_BYTES;
  uint16 tileRowAddr = tileAddr + 2 * row;
  uint8 rowDataLo = Memory::getByte(tileRowAddr);
  uint8 rowDataHi = Memory::getByte(tileRowAddr + 1);

  // convert tile row data into pixel values
  // rowDataLo = abcdefgh
  // rowDataHi = ijklmnop
  // tileRow = {ia, jb, kc, ld, me, nf, og, ph}
  TileRow tileRow;
  for (int px = 0; px < TILE_PX_DIM; px++) {
    uint8 pxShift = TILE_PX_DIM - px - 1;
    tileRow[px] = (rowDataLo >> pxShift) & 0b1;
    tileRow[px] |= ((rowDataHi >> pxShift) << 1) & 0b10;
  }
  return tileRow;
}

// get specified row of given sprite
TileRow PPU::getSpriteRow(sprite_t oamEntry, uint8 row) {
  uint8 height = spriteHeight();
  row = oamEntry.flipY ? height - row - 1 : row;
  uint8 pattern = height == SPRITE_PX_HEIGHT_TALL ? oamEntry.pattern & 0xFE
                                                  : oamEntry.pattern;
  TileRow tileRow = getTileRow(BG_DATA_ADDR_1, pattern, row);
  if (oamEntry.flipX) {
    for (uint8 i = 0; i < tileRow.size() / 2; ++i) {
      uint8 swapIdx = tileRow.size() - i - 1;
      uint8 temp = tileRow[swapIdx];
      tileRow[swapIdx] = tileRow[i];
      tileRow[i] = temp;
    }
  }
  return tileRow;
}

// get sprite oam entry at a given sprite index
sprite_t PPU::getSpriteOAM(uint8 spriteIdx) {
  sprite_t sprite;
  uint16 spriteAddr = OAM_ADDR + spriteIdx * OAM_ENTRY_BYTES;

  // get sprite position and pattern
  sprite.y = Memory::getByte(spriteAddr);
  sprite.x = Memory::getByte(spriteAddr + 1);
  sprite.pattern = Memory::getByte(spriteAddr + 2);

  // get sprite flags
  uint8 flags = Memory::getByte(spriteAddr + 3);
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
  return lcdc & 0x40 ? WINDOW_MAP_ADDR_1 : WINDOW_MAP_ADDR_0;
}

uint16 PPU::bgWindowDataAddr() {
  return lcdc & 0x10 ? BG_DATA_ADDR_1 : BG_DATA_ADDR_0;
}

uint16 PPU::bgMapAddr() { return lcdc & 0x08 ? BG_MAP_ADDR_1 : BG_MAP_ADDR_0; }

// **************************************************
// **************************************************
// STAT Register Functions
// **************************************************
// **************************************************

// set the current lcd mode
// 00 - hblank mode
// 01 - vblank mode
// 10 - oam search mode
// 11 - pixel transfer mode
void PPU::setMode(uint8 mode) {
  stat &= 0xFC;
  stat |= mode;
}

uint8 PPU::getMode() { return stat & 0b11; }

// set the interrupt register IF based on
// the current state of the STAT register
void PPU::setLCDInterrupt() {
  if (((stat & 0x40) && (stat & 0b100)) ||
      ((stat & 0x20) && (stat & 0b011) == OAM_SEARCH_MODE) ||
      ((stat & 0x10) && (stat & 0b011) == V_BLANK_MODE) ||
      ((stat & 0x08) && (stat & 0b011) == H_BLANK_MODE)) {
    Interrupts::request(LCDC_INT);
  }
}
