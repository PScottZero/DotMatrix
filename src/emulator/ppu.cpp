// **************************************************
// **************************************************
// **************************************************
// Pixel Processing Unit (PPU)
// **************************************************
// **************************************************
// **************************************************

#include "ppu.h"

#include "cgb.h"
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
uint8 PPU::windowLineNum = 0;

// initialize visible sprite
// array and count
sprite_t PPU::visibleSprites[MAX_SPRITES_PER_LINE]{};
uint8 PPU::visibleSpriteCount = 0;

// initialize palette
Palette *PPU::palette = nullptr;

// initialize frame rendered flag
bool PPU::frameRendered = false;

// initialize show/hide flags
bool PPU::showBackground = true;
bool PPU::showWindow = true;
bool PPU::showSprites = true;

// initialize stat register interrupt
bool PPU::statInt = false;

void PPU::step() {
  if (lcdEnable() && !CGB::stop) {
    // if scanline completed, increment ly
    if (CycleCounter::ppuCycles > SCANLINE_CYCLES) {
      if (++ly >= SCREEN_LINES) ly = 0;

      // check if current line number
      // is equal to the value in lyc
      if (ly == lyc) {
        stat |= BIT2_MASK;
      } else {
        stat &= ~BIT2_MASK;
      }
      setLcdInterrupt();

      CycleCounter::ppuCycles %= SCANLINE_CYCLES;
    }

    if (ly < SCREEN_PX_HEIGHT) {
      // **************************************************
      // OAM Search
      // **************************************************
      if (CycleCounter::ppuCycles < OAM_SEARCH_CYCLES) {
        if (getMode() != OAM_SEARCH_MODE) {
          setMode(OAM_SEARCH_MODE);
          setLcdInterrupt();
          findVisibleSprites();
        }
      }

      // **************************************************
      // Pixel Transfer
      // **************************************************
      else if (CycleCounter::ppuCycles < PIXEL_TRANSFER_CYCLES) {
        if (getMode() != PIXEL_TRANSFER_MODE) {
          setMode(PIXEL_TRANSFER_MODE);
          setLcdInterrupt();

          scanline_t scanline;
          resetScanline(scanline);
          if (bgEnable()) renderBg(scanline);
          if (windowEnable()) renderWindow(scanline);
          if (spriteEnable()) renderSprites(scanline);
          transferScanlineToScreen(scanline);
        }
      }

      // **************************************************
      // H-Blank
      // **************************************************
      else if (CycleCounter::ppuCycles < H_BLANK_CYCLES) {
        if (getMode() != H_BLANK_MODE) {
          setMode(H_BLANK_MODE);
          setLcdInterrupt();
        }
      }
    } else {
      // **************************************************
      // V-Blank
      // **************************************************
      if (getMode() != V_BLANK_MODE) {
        setMode(V_BLANK_MODE);
        setLcdInterrupt();
        Interrupts::request(V_BLANK_INT);
        frameRendered = true;
        windowLineNum = 0;
      }
    }
  } else {
    ly = 0;
    windowLineNum = 0;
    stat &= ~THREE_BITS_MASK;
    statInt = false;
    if (CycleCounter::ppuCycles > SCANLINE_CYCLES * SCREEN_LINES) {
      screen->fill(palette->data[0]);
      CycleCounter::ppuCycles %= SCANLINE_CYCLES * SCREEN_LINES;
      frameRendered = true;
    }
  }
}

// **************************************************
// **************************************************
// OAM Search Functions
// **************************************************
// **************************************************

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

// **************************************************
// **************************************************
// Rendering Functions
// **************************************************
// **************************************************

// render background tile rows that
// intersect current scanline
void PPU::renderBg(scanline_t &scanline) {
  uint16 tileMapAddr = bgMapAddr();
  uint16 tileDataAddr = bgWindowDataAddr();

  // render background row
  uint8 pxCount = 0;
  uint8 pxY = scy + ly;
  while (pxCount < SCREEN_PX_WIDTH) {
    // get background tile number (0 to 1023)
    uint8 pxX = scx + pxCount;
    uint8 bgTileX = pxX / TILE_PX_DIM;
    uint8 bgTileY = pxY / TILE_PX_DIM;
    uint8 innerBgTileX = pxX % TILE_PX_DIM;
    uint8 innerBgTileY = pxY % TILE_PX_DIM;
    uint16 bgTileNo = bgTileY * BG_TILE_DIM + bgTileX;

    // get data tile number (0 to 256 or -128 to 127)
    uint8 tileNo = Memory::getByte(tileMapAddr + bgTileNo);

    // get row of pixels from tile
    TileRow row = getTileRow(tileDataAddr, tileNo, innerBgTileY);

    // transfer pixel row to scanline
    int start = pxCount == 0 ? innerBgTileX : 0;
    int end =
        pxCount + 8 > SCREEN_PX_WIDTH ? SCREEN_PX_WIDTH - pxCount : TILE_PX_DIM;
    for (int i = start; i < end; ++i) {
      scanline.pixels[pxCount] = row[i];
      scanline.palettes[pxCount] = PaletteType::BG;
      ++pxCount;
    }
  }
}

// render sprite tile rows that
// intersect the current scanline
void PPU::renderSprites(scanline_t &scanline) {
  for (int spriteIdx = 0; spriteIdx < visibleSpriteCount; ++spriteIdx) {
    sprite_t sprite = visibleSprites[spriteIdx];
    uint8 spriteRow = (ly + 16) - sprite.y;
    TileRow row = getSpriteRow(sprite, spriteRow);

    // draw sprite row onto screen
    for (int rowIdx = 0; rowIdx < TILE_PX_DIM; ++rowIdx) {
      int pxX = sprite.x + rowIdx - 8;
      if (pxX >= 0 && pxX < SCREEN_PX_WIDTH) {
        if (shouldDrawSpritePixel(sprite, scanline, pxX, row[rowIdx])) {
          scanline.pixels[pxX] = row[rowIdx];
          scanline.palettes[pxX] =
              sprite.palette ? PaletteType::SPRITE1 : PaletteType::SPRITE0;
          scanline.spriteIndices[pxX] = spriteIdx;
        }
      }
    }
  }
}

// render window tile rows that
// intersect the current scanline
void PPU::renderWindow(scanline_t &scanline) {
  if (ly >= wy) {
    uint16 tileMapAddr = windowMapAddr();
    uint16 tileDataAddr = bgWindowDataAddr();

    uint8 pxCount = 0;
    uint8 winX = wx - 7;
    while (pxCount + winX < SCREEN_PX_WIDTH) {
      uint8 winTileX = pxCount / TILE_PX_DIM;
      uint8 winTileY = windowLineNum / TILE_PX_DIM;
      uint16 winTileNo = winTileY * BG_TILE_DIM + winTileX;
      uint8 tileNo = Memory::getByte(tileMapAddr + winTileNo);

      TileRow row = getTileRow(tileDataAddr, tileNo, windowLineNum % 8);
      for (int i = 0; i < TILE_PX_DIM; ++i) {
        if (winX + pxCount >= 0 && winX + pxCount < SCREEN_PX_WIDTH) {
          scanline.pixels[winX + pxCount] = row[i];
          scanline.palettes[winX + pxCount] = PaletteType::BG;
        }
        if (++pxCount + winX >= SCREEN_PX_WIDTH) break;
      }
    }

    ++windowLineNum;
  }
}

bool PPU::shouldDrawSpritePixel(sprite_t &sprite, scanline_t &scanline,
                                uint8 scanlineIdx, uint8 px) {
  // if another sprite is located at the current pixel,
  // draw current sprite if it has a lower x value
  if (scanline.palettes[scanlineIdx] == PaletteType::SPRITE0 ||
      scanline.palettes[scanlineIdx] == PaletteType::SPRITE1) {
    auto otherSprite = visibleSprites[scanline.spriteIndices[scanlineIdx]];
    return sprite.x < otherSprite.x && px != 0;
  }

  // if sprite priority is true, then only draw sprite
  // if current scaline color is zero, otherwise draw
  // sprite (in both bases ignoring px values of 0)
  return (!sprite.priority || scanline.pixels[scanlineIdx] == 0) && px != 0;
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
    uint8 pxPalVal = (pal >> (2 * scanline.pixels[px])) & TWO_BITS_MASK;
    screen->setPixel(px, ly, palette->data[pxPalVal]);
  }
}

// reset scanline by zeroing it out
void PPU::resetScanline(scanline_t &scanline) {
  for (int i = 0; i < SCREEN_PX_WIDTH; ++i) {
    scanline.pixels[i] = 0;
    scanline.palettes[i] = PaletteType::BG;
    scanline.spriteIndices[i] = 0;
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
    tileRow[px] = (rowDataLo >> pxShift) & BIT0_MASK;
    tileRow[px] |= ((rowDataHi >> pxShift) << 1) & BIT1_MASK;
  }
  return tileRow;
}

// get specified row of given sprite
TileRow PPU::getSpriteRow(sprite_t oamEntry, uint8 row) {
  uint8 height = spriteHeight();
  row = oamEntry.flipY ? height - row - 1 : row;
  uint8 pattern = height == SPRITE_PX_HEIGHT_TALL
                      ? oamEntry.pattern & ~BIT0_MASK
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
  sprite.priority = flags & BIT7_MASK;
  sprite.flipY = flags & BIT6_MASK;
  sprite.flipX = flags & BIT5_MASK;
  sprite.palette = flags & BIT4_MASK;

  return sprite;
}

// get background map attribute for a given tile in
// the background map
bg_map_attr_t PPU::getBgMapAttr(uint8 tileNo) {
  bg_map_attr_t attr;
  uint8 attrByte = Memory::getByte(bgMapAddr() + tileNo);
  attr.priority = attrByte & BIT7_MASK;
  attr.flipY = attrByte & BIT6_MASK;
  attr.flipX = attrByte & BIT5_MASK;
  attr.tileVramBankNum = attrByte & BIT3_MASK;
  attr.bgPaletteNum = attrByte & THREE_BITS_MASK;
  return attr;
}

// **************************************************
// **************************************************
// LCDC Register Functions
// **************************************************
// **************************************************

bool PPU::lcdEnable() { return lcdc & BIT7_MASK; }

bool PPU::bgEnable() { return (lcdc & BIT0_MASK) && showBackground; }

bool PPU::windowEnable() { return (lcdc & BIT5_MASK) && showWindow; }

bool PPU::spriteEnable() { return (lcdc & BIT1_MASK) && showSprites; }

uint8 PPU::spriteHeight() {
  return lcdc & BIT2_MASK ? SPRITE_PX_HEIGHT_TALL : SPRITE_PX_HEIGHT_SHORT;
}

uint16 PPU::windowMapAddr() {
  return lcdc & BIT6_MASK ? WINDOW_MAP_ADDR_1 : WINDOW_MAP_ADDR_0;
}

uint16 PPU::bgWindowDataAddr() {
  return lcdc & BIT4_MASK ? BG_DATA_ADDR_1 : BG_DATA_ADDR_0;
}

uint16 PPU::bgMapAddr() {
  return lcdc & BIT3_MASK ? BG_MAP_ADDR_1 : BG_MAP_ADDR_0;
}

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
  stat &= ~TWO_BITS_MASK;
  stat |= mode;
}

// return the current lcd mode
// 00 - hblank mode
// 01 - vblank mode
// 10 - oam search mode
// 11 - pixel transfer mode
uint8 PPU::getMode() { return stat & TWO_BITS_MASK; }

bool PPU::coincidenceIntEnabled() { return stat & BIT6_MASK; }

bool PPU::oamSearchIntEnabled() { return stat & BIT5_MASK; }

bool PPU::vblankIntEnabled() { return stat & BIT4_MASK; }

bool PPU::hblankIntEnabled() { return stat & BIT3_MASK; }

bool PPU::lyEqualsLyc() { return stat & BIT2_MASK; }

// set the interrupt register IF based on
// the current state of the STAT register
void PPU::setLcdInterrupt() {
  if ((coincidenceIntEnabled() && lyEqualsLyc()) ||
      (oamSearchIntEnabled() && getMode() == OAM_SEARCH_MODE) ||
      ((vblankIntEnabled() || oamSearchIntEnabled()) &&
       getMode() == V_BLANK_MODE) ||
      (hblankIntEnabled() && getMode() == H_BLANK_MODE)) {
    // only request an interrupt if stat
    // interrupt goes from low to high
    if (!statInt) {
      statInt = true;
      Interrupts::request(LCDC_INT);
    }
  } else {
    statInt = false;
  }
}
