// **************************************************
// **************************************************
// **************************************************
// Pixel Processing Unit (PPU)
// **************************************************
// **************************************************
// **************************************************

#include "ppu.h"

#include "cgb.h"
#include "interrupts.h"
#include "memory.h"

PPU::PPU()
    : cgb(nullptr),
      screen(nullptr),
      palette(nullptr),
      lcdc(nullptr),
      stat(nullptr),
      scy(nullptr),
      scx(nullptr),
      ly(nullptr),
      lyc(nullptr),
      bgp(nullptr),
      obp0(nullptr),
      obp1(nullptr),
      wx(nullptr),
      wy(nullptr),
      windowLineNum(0),
      visibleSprites{},
      visibleSpriteCount(0),
      frameRendered(false),
      showBackground(true),
      showWindow(true),
      showSprites(true),
      statInt(false),
      cycles(0) {}

void PPU::step() {
  cycles += 1;
  if (lcdEnable() && !cgb->stop) {
    // if scanline completed, increment ly
    if (cycles > SCANLINE_CYCLES) {
      if (++*ly >= SCREEN_LINES) *ly = 0;

      // check if current line number
      // is equal to the value in lyc
      if (*ly == *lyc) {
        *stat |= BIT2_MASK;
      } else {
        *stat &= ~BIT2_MASK;
      }
      setLcdStatInterrupt();

      cycles %= SCANLINE_CYCLES;
    }

    if (*ly < SCREEN_PX_HEIGHT) {
      // **************************************************
      // OAM Search
      // **************************************************
      if (cycles < OAM_SEARCH_CYCLES) {
        if (getMode() != OAM_SEARCH_MODE) {
          setMode(OAM_SEARCH_MODE);
          setLcdStatInterrupt();
          findVisibleSprites();
        }
      }

      // **************************************************
      // Pixel Transfer
      // **************************************************
      else if (cycles < PIXEL_TRANSFER_CYCLES) {
        if (getMode() != PIXEL_TRANSFER_MODE) {
          setMode(PIXEL_TRANSFER_MODE);
          setLcdStatInterrupt();

          scanline_t scanline;
          resetScanline(scanline);
          if ((bgEnable() || !cgb->dmgMode) && showBackground)
            renderBg(scanline);
          if (windowEnable() && showWindow) renderWindow(scanline);
          if (spriteEnable() && showSprites) renderSprites(scanline);
          transferScanlineToScreen(scanline);
        }
      }

      // **************************************************
      // H-Blank
      // **************************************************
      else if (cycles < H_BLANK_CYCLES) {
        if (getMode() != H_BLANK_MODE) {
          setMode(H_BLANK_MODE);
          setLcdStatInterrupt();
        }
      }
    } else {
      // **************************************************
      // V-Blank
      // **************************************************
      if (getMode() != V_BLANK_MODE) {
        setMode(V_BLANK_MODE);
        setLcdStatInterrupt();
        cgb->interrupts.request(V_BLANK_INT);
        frameRendered = true;
        windowLineNum = 0;
      }
    }
  } else {
    *ly = 0;
    windowLineNum = 0;
    *stat &= ~THREE_BITS_MASK;
    statInt = false;
    if (cycles > SCANLINE_CYCLES * SCREEN_LINES) {
      if (cgb->dmgMode) screen->fill(palette->data[0]);
      cycles %= SCANLINE_CYCLES * SCREEN_LINES;
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
    if ((*ly + SPRITE_PX_HEIGHT_TALL) >= oamEntry.y &&
        (*ly + SPRITE_PX_HEIGHT_TALL) < (oamEntry.y + spriteHeight())) {
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
  uint8 pxY = *scy + *ly;
  while (pxCount < SCREEN_PX_WIDTH) {
    // get background tile number (0 to 1023)
    uint8 pxX = *scx + pxCount;
    uint8 bgTileX = pxX / TILE_PX_DIM;
    uint8 bgTileY = pxY / TILE_PX_DIM;
    uint8 innerBgTileX = pxX % TILE_PX_DIM;
    uint8 innerBgTileY = pxY % TILE_PX_DIM;
    uint16 bgTileNo = bgTileY * BG_TILE_DIM + bgTileX;

    // get data tile number (0 to 256 or -128 to 127)
    uint8 tileNo = cgb->mem.getVramByte(tileMapAddr + bgTileNo, false);

    TileRow row;
    tile_map_attr_t attr;
    attr.priority = 0;
    attr.paletteNum = 0;

    // dmg get row of tile pixels
    if (cgb->dmgMode) {
      row = getTileRow(tileDataAddr, tileNo, innerBgTileY);
    }

    // cgb get row of tile pixels
    else {
      attr = getTileMapAttr(tileMapAddr, bgTileNo);
      row = getTileRow(attr, tileNo, innerBgTileY);
    }

    // transfer pixel row to scanline
    int start = pxCount == 0 ? innerBgTileX : 0;
    int end =
        pxCount + 8 > SCREEN_PX_WIDTH ? SCREEN_PX_WIDTH - pxCount : TILE_PX_DIM;
    for (int i = start; i < end; ++i) {
      scanline.pixels[pxCount] = row[i];
      scanline.paletteTypes[pxCount] = PaletteType::BG;
      if (!cgb->dmgMode) {
        scanline.paletteIndices[pxCount] = attr.paletteNum;
        scanline.priorities[pxCount] = attr.priority;
      }
      ++pxCount;
    }
  }
}

// render window tile rows that
// intersect the current scanline
void PPU::renderWindow(scanline_t &scanline) {
  if (*ly >= *wy) {
    uint16 tileMapAddr = windowMapAddr();
    uint16 tileDataAddr = bgWindowDataAddr();

    uint8 pxCount = 0;
    uint8 winX = *wx - 7;
    while (pxCount + winX < SCREEN_PX_WIDTH) {
      uint8 winTileX = pxCount / TILE_PX_DIM;
      uint8 winTileY = windowLineNum / TILE_PX_DIM;
      uint16 winTileNo = winTileY * BG_TILE_DIM + winTileX;
      uint8 tileNo = cgb->mem.getVramByte(tileMapAddr + winTileNo, false);

      TileRow row;
      tile_map_attr_t attr;
      attr.priority = 0;
      attr.paletteNum = 0;

      // dmg get tile row of pixels
      if (cgb->dmgMode) {
        row = getTileRow(tileDataAddr, tileNo, windowLineNum % 8);
      }

      // cgb get tile row of tile pixels
      else {
        attr = getTileMapAttr(tileMapAddr, winTileNo);
        row = getTileRow(attr, tileNo, windowLineNum % 8);
      }

      // transfer pixel row to scanline
      for (int i = 0; i < TILE_PX_DIM; ++i) {
        if (winX + pxCount >= 0 && winX + pxCount < SCREEN_PX_WIDTH) {
          scanline.pixels[winX + pxCount] = row[i];
          scanline.paletteTypes[winX + pxCount] = PaletteType::BG;
          if (!cgb->dmgMode) {
            scanline.paletteIndices[pxCount] = attr.paletteNum;
            scanline.priorities[pxCount] = attr.priority;
          }
        }
        if (++pxCount + winX >= SCREEN_PX_WIDTH) break;
      }
    }

    ++windowLineNum;
  }
}

// render sprite tile rows that
// intersect the current scanline
void PPU::renderSprites(scanline_t &scanline) {
  for (int spriteIdx = 0; spriteIdx < visibleSpriteCount; ++spriteIdx) {
    sprite_t sprite = visibleSprites[spriteIdx];
    uint8 spriteRow = (*ly + 16) - sprite.y;
    TileRow row = getSpriteRow(sprite, spriteRow);

    // draw sprite row onto screen
    for (int rowIdx = 0; rowIdx < TILE_PX_DIM; ++rowIdx) {
      int pxX = sprite.x + rowIdx - 8;
      if (pxX >= 0 && pxX < SCREEN_PX_WIDTH) {
        if (spriteHasPriority(sprite, scanline, pxX, row[rowIdx])) {
          scanline.pixels[pxX] = row[rowIdx];
          scanline.paletteTypes[pxX] =
              sprite.palette ? PaletteType::SPRITE1 : PaletteType::SPRITE0;
          scanline.spriteIndices[pxX] = spriteIdx;
          if (!cgb->dmgMode) {
            scanline.paletteIndices[pxX] = sprite.paletteNum;
            scanline.priorities[pxX] = sprite.priority;
          }
        }
      }
    }
  }
}

// check sprite/background priority to determine if
// sprite pixel should be rendered
bool PPU::spriteHasPriority(sprite_t &sprite, scanline_t &scanline,
                            uint8 scanlineIdx, uint8 px) {
  // if another sprite is located at the current pixel,
  // draw current sprite if it has a lower x value
  if (scanline.paletteTypes[scanlineIdx] == PaletteType::SPRITE0 ||
      scanline.paletteTypes[scanlineIdx] == PaletteType::SPRITE1) {
    auto otherSprite = visibleSprites[scanline.spriteIndices[scanlineIdx]];
    return sprite.x < otherSprite.x && px != 0;
  }

  // dmg background/sprite priority
  if (cgb->dmgMode) {
    // if sprite priority is true, then only draw sprite
    // if current scaline color is zero, otherwise draw
    // sprite (in both bases ignoring px values of 0)
    return (!sprite.priority || scanline.pixels[scanlineIdx] == 0) && px != 0;
  }

  // cgb background/sprite priority
  else {
    return (!bgEnable() ||
            (!scanline.priorities[scanlineIdx] && !sprite.priority) ||
            scanline.pixels[scanlineIdx] == 0) &&
           px != 0;
  }
}

// apply palette colors to current pixel
// values in the screen buffer
void PPU::transferScanlineToScreen(scanline_t &scanline) {
  for (int px = 0; px < SCREEN_PX_WIDTH; ++px) {
    auto palType = scanline.paletteTypes[px];
    uint pxColor;
    if (cgb->dmgMode) {
      uint8 pal;
      switch (palType) {
        case PaletteType::BG:
          pal = *bgp;
          break;
        case PaletteType::SPRITE0:
          pal = *obp0;
          break;
        case PaletteType::SPRITE1:
          pal = *obp1;
          break;
      }
      uint8 pxPalVal = (pal >> (2 * scanline.pixels[px])) & TWO_BITS_MASK;
      pxColor = palette->data[pxPalVal];
    } else {
      uint8 palIdx = scanline.paletteIndices[px];
      uint8 *cram =
          palType == PaletteType::BG ? cgb->mem.cramBg : cgb->mem.cramObj;
      pxColor = getPaletteColor(cram, palIdx, scanline.pixels[px]);
    }
    screen->setPixel(px, *ly, pxColor);
  }
}

// reset scanline by zeroing it out
void PPU::resetScanline(scanline_t &scanline) {
  for (int i = 0; i < SCREEN_PX_WIDTH; ++i) {
    scanline.pixels[i] = 0;
    scanline.paletteTypes[i] = PaletteType::BG;
    scanline.spriteIndices[i] = 0;
    scanline.paletteIndices[i] = 0;
    scanline.priorities[i] = false;
  }
}

// **************************************************
// **************************************************
// Read Display Memory Functions
// **************************************************
// **************************************************

// get specified row of the given tile
TileRow PPU::getTileRow(uint16 baseAddr, uint8 tileNo, uint8 row,
                        bool vramBank) {
  // get tile row data
  int16 tileNoSigned = baseAddr == BG_DATA_ADDR_0 ? (int8)tileNo : tileNo;
  uint16 tileAddr = baseAddr + tileNoSigned * TILE_BYTES;
  uint16 tileRowAddr = tileAddr + 2 * row;
  uint8 rowDataLo = cgb->mem.getVramByte(tileRowAddr, vramBank);
  uint8 rowDataHi = cgb->mem.getVramByte(tileRowAddr + 1, vramBank);

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

// get specified row of the given background
// or window tile (cgb only)
TileRow PPU::getTileRow(tile_map_attr_t tileMapAttr, uint8 tileNo, uint8 row) {
  row = tileMapAttr.flipY ? TILE_PX_DIM - row - 1 : row;
  TileRow tileRow =
      getTileRow(bgWindowDataAddr(), tileNo, row, tileMapAttr.vramBankNum);
  if (tileMapAttr.flipX) flipTileRow(tileRow);
  return tileRow;
}

// get specified row of given sprite
TileRow PPU::getSpriteRow(sprite_t oamEntry, uint8 row) {
  uint8 height = spriteHeight();
  row = oamEntry.flipY ? height - row - 1 : row;
  uint8 pattern = height == SPRITE_PX_HEIGHT_TALL
                      ? oamEntry.pattern & ~BIT0_MASK
                      : oamEntry.pattern;
  TileRow tileRow =
      getTileRow(BG_DATA_ADDR_1, pattern, row, oamEntry.vramBankNum);
  if (oamEntry.flipX) flipTileRow(tileRow);
  return tileRow;
}

// get sprite oam entry at a given sprite index
sprite_t PPU::getSpriteOAM(uint8 spriteIdx) {
  sprite_t sprite;
  uint16 spriteAddr = OAM_ADDR + spriteIdx * OAM_ENTRY_BYTES;

  // get sprite position and pattern
  sprite.y = cgb->mem.getByte(spriteAddr);
  sprite.x = cgb->mem.getByte(spriteAddr + 1);
  sprite.pattern = cgb->mem.getByte(spriteAddr + 2);

  // get sprite flags
  uint8 flags = cgb->mem.getByte(spriteAddr + 3);
  sprite.priority = flags & BIT7_MASK;
  sprite.flipY = flags & BIT6_MASK;
  sprite.flipX = flags & BIT5_MASK;
  sprite.palette = flags & BIT4_MASK;

  // cgb additional sprite flags
  sprite.vramBankNum = flags & BIT3_MASK;
  sprite.paletteNum = flags & THREE_BITS_MASK;

  return sprite;
}

// get background map attribute for a given tile in
// the background map (cgb only)
tile_map_attr_t PPU::getTileMapAttr(uint16 baseAddr, uint16 bgWinTileNo) {
  tile_map_attr_t attr;
  uint8 attrByte = cgb->mem.getVramByte(baseAddr + bgWinTileNo, true);
  attr.priority = attrByte & BIT7_MASK;
  attr.flipY = attrByte & BIT6_MASK;
  attr.flipX = attrByte & BIT5_MASK;
  attr.vramBankNum = attrByte & BIT3_MASK;
  attr.paletteNum = attrByte & THREE_BITS_MASK;
  return attr;
}

// flips the given tile row
// pixels before: a b c d e f g h
// pixels after:  h g f e d c b a
void PPU::flipTileRow(TileRow &row) {
  for (uint8 i = 0; i < row.size() / 2; ++i) {
    uint8 swapIdx = row.size() - i - 1;
    uint8 temp = row[swapIdx];
    row[swapIdx] = row[i];
    row[i] = temp;
  }
}

// **************************************************
// **************************************************
// LCDC Register Functions
// **************************************************
// **************************************************

bool PPU::lcdEnable() { return *lcdc & BIT7_MASK; }

bool PPU::bgEnable() { return (*lcdc & BIT0_MASK); }

bool PPU::windowEnable() { return (*lcdc & BIT0_MASK) && (*lcdc & BIT5_MASK); }

bool PPU::spriteEnable() { return (*lcdc & BIT1_MASK); }

uint8 PPU::spriteHeight() {
  return *lcdc & BIT2_MASK ? SPRITE_PX_HEIGHT_TALL : SPRITE_PX_HEIGHT_SHORT;
}

uint16 PPU::windowMapAddr() {
  return *lcdc & BIT6_MASK ? WINDOW_MAP_ADDR_1 : WINDOW_MAP_ADDR_0;
}

uint16 PPU::bgWindowDataAddr() {
  return *lcdc & BIT4_MASK ? BG_DATA_ADDR_1 : BG_DATA_ADDR_0;
}

uint16 PPU::bgMapAddr() {
  return *lcdc & BIT3_MASK ? BG_MAP_ADDR_1 : BG_MAP_ADDR_0;
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
  *stat &= ~TWO_BITS_MASK;
  *stat |= mode;
}

// return the current lcd mode
// 00 - hblank mode
// 01 - vblank mode
// 10 - oam search mode
// 11 - pixel transfer mode
uint8 PPU::getMode() { return *stat & TWO_BITS_MASK; }

bool PPU::coincidenceIntEnabled() { return *stat & BIT6_MASK; }

bool PPU::oamSearchIntEnabled() { return *stat & BIT5_MASK; }

bool PPU::vblankIntEnabled() { return *stat & BIT4_MASK; }

bool PPU::hblankIntEnabled() { return *stat & BIT3_MASK; }

bool PPU::lyEqualsLyc() { return *stat & BIT2_MASK; }

// set the interrupt register IF based on
// the current state of the STAT register
void PPU::setLcdStatInterrupt() {
  if ((coincidenceIntEnabled() && lyEqualsLyc()) ||
      (oamSearchIntEnabled() && getMode() == OAM_SEARCH_MODE) ||
      ((vblankIntEnabled() || oamSearchIntEnabled()) &&
       getMode() == V_BLANK_MODE) ||
      (hblankIntEnabled() && getMode() == H_BLANK_MODE)) {
    // only request an interrupt if stat
    // interrupt goes from low to high
    if (!statInt) {
      statInt = true;
      cgb->interrupts.request(LCDC_INT);
    }
  } else {
    statInt = false;
  }
}

// **************************************************
// **************************************************
// LCD Color Palette Functions (CGB Only)
// **************************************************
// **************************************************

uint PPU::getPaletteColor(uint8 *cram, uint8 palIdx, uint8 colorIdx) {
  // get color of palette
  uint8 cramAddr = palIdx * PAL_SIZE + colorIdx * 2;
  uint16 color = cram[cramAddr + 1] << 8 | cram[cramAddr];

  // get color channels
  uint8 red = color & FIVE_BITS_MASK;
  uint8 green = (color >> 5) & FIVE_BITS_MASK;
  uint8 blue = (color >> 10) & FIVE_BITS_MASK;

  // convert to 32-bit argb color
  return qRgb(red * 8, green * 8, blue * 8);
}
