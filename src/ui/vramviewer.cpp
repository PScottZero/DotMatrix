#include "vramviewer.h"

#include <chrono>
#include <thread>

#include "ui_vramviewer.h"

using namespace chrono;

VramViewer::VramViewer(CGB *cgb, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::VramViewer),
      cgb(cgb),
      running(true),
      vramDisplay(TILE_PX_DIM * VIEWER_TILES_PER_ROW,
                  TILE_PX_DIM * VIEWER_TILE_ROWS, QImage::Format_RGB32),
      bgDisplay(BG_PX_DIM, BG_PX_DIM, QImage::Format_RGB32),
      winDisplay(BG_PX_DIM, BG_PX_DIM, QImage::Format_RGB32) {
  ui->setupUi(this);
  ui->tabWidget->setCurrentIndex(0);
  std::thread renderThread(&VramViewer::render, this);
  renderThread.detach();
}

VramViewer::~VramViewer() {
  running = false;
  delete ui;
}

void VramViewer::render() {
  auto clock = system_clock::now();
  while (running) {
    QImage *image = nullptr;
    QLabel *display = nullptr;
    switch (ui->tabWidget->currentIndex()) {
      case TILES_TAB:
        renderTiles();
        image = &vramDisplay;
        display = ui->vramDisplay;
        break;
      case BG_TAB:
        renderBackground();
        image = &bgDisplay;
        display = ui->bgDisplay;
        break;
      case WIN_TAB:
        renderWindow();
        image = &winDisplay;
        display = ui->winDisplay;
        break;
    }

    if (display && image) {
      auto pixmap = QPixmap::fromImage(*image).scaled(
          display->width(), display->height(), Qt::KeepAspectRatio,
          Qt::FastTransformation);
      display->setPixmap(pixmap);
    }

    long long duration = FRAME_DURATION;
    clock += microseconds(duration);
    this_thread::sleep_until(clock);
  }
}

void VramViewer::renderTiles() {
  if (cgb->running) {
    for (int vramBank = 0; vramBank < 2; ++vramBank) {
      for (int tile = 0; tile < VRAM_TILE_COUNT; ++tile) {
        uint16 baseAddr = tile < 256 ? TILE_DATA_ADDR_1 : TILE_DATA_ADDR_0;
        uint8 tileNo = tile < 256 ? tile : tile - 256;
        uint16 tileX = (tile % 16) * TILE_PX_DIM;
        uint16 tileY = (tile / 16) * TILE_PX_DIM;
        for (int row = 0; row < TILE_PX_DIM; ++row) {
          auto pixelRow = cgb->ppu.getTileRow(baseAddr, tileNo, row, vramBank);
          for (int px = 0; px < TILE_PX_DIM; ++px) {
            auto pixelColor = cgb->ppu.palette->data[pixelRow[px]];
            vramDisplay.setPixel(tileX + px + (vramBank ? 128 : 0), tileY + row,
                                 pixelColor);
          }
        }
      }
    }
  } else {
    vramDisplay.fill(cgb->ppu.palette->data[0]);
  }
}

void VramViewer::renderBackground() {
  renderTileMap(bgDisplay, cgb->ppu.bgMapAddr());
  renderBgViewport();
}

void VramViewer::renderWindow() {
  renderTileMap(winDisplay, cgb->ppu.windowMapAddr());
}

void VramViewer::renderTileMap(QImage &display, uint16 tileMapAddr) {
  if (cgb->running) {
    for (int tile = 0; tile < BG_TILE_DIM * BG_TILE_DIM; ++tile) {
      uint8 tileNo = cgb->mem.getVramByte(tileMapAddr + tile, false);
      auto attr = cgb->ppu.getTileMapAttr(tileMapAddr, tile);
      uint8 tileX = (tile % BG_TILE_DIM) * TILE_PX_DIM;
      uint8 tileY = (tile / BG_TILE_DIM) * TILE_PX_DIM;
      for (int row = 0; row < TILE_PX_DIM; ++row) {
        auto pixelRow =
            cgb->cgbMode
                ? cgb->ppu.getTileRow(attr, tileNo, row)
                : cgb->ppu.getTileRow(cgb->ppu.bgWindowDataAddr(), tileNo, row);
        for (int px = 0; px < TILE_PX_DIM; ++px) {
          uint pixelColor;
          if (cgb->cgbMode) {
            auto paletteNum = cgb->dmgMode ? 0 : attr.paletteNum;
            pixelColor = cgb->ppu.getPaletteColor(cgb->mem.cramBg, paletteNum,
                                                  pixelRow[px]);
          } else {
            pixelColor =
                cgb->ppu.getPaletteColor(cgb->mem.getByte(BGP), pixelRow[px]);
          }
          display.setPixel(tileX + px, tileY + row, pixelColor);
        }
      }
    }
  } else {
    display.fill(cgb->cgbMode ? cgb->ppu.getPaletteColor(cgb->mem.cramBg, 0, 0)
                              : cgb->ppu.palette->data[0]);
  }
}

void VramViewer::renderBgViewport() {
  uint8 scx = cgb->ppu.renderedScx;
  uint8 scy = cgb->ppu.renderedScy;

  // top and bottom lines of viewport
  for (uint8 x = 0; x < SCREEN_PX_WIDTH; ++x) {
    uint8 bothX = scx + x;
    uint8 topY = scy;
    uint8 bottomY = scy + SCREEN_PX_HEIGHT - 1;
    uint topColor = bgDisplay.pixel(bothX, topY);
    uint bottomColor = bgDisplay.pixel(bothX, bottomY);
    bgDisplay.setPixel(bothX, topY, 0xFF000000 | ~topColor);
    bgDisplay.setPixel(bothX, bottomY, 0xFF000000 | ~bottomColor);
  }

  // left and right lines of viewport
  for (uint8 y = 1; y < SCREEN_PX_HEIGHT - 1; ++y) {
    uint8 bothY = scy + y;
    uint8 leftX = scx;
    uint8 rightX = scx + SCREEN_PX_WIDTH - 1;
    uint leftColor = bgDisplay.pixel(leftX, bothY);
    uint rightColor = bgDisplay.pixel(rightX, bothY);
    bgDisplay.setPixel(leftX, bothY, 0xFF000000 | ~leftColor);
    bgDisplay.setPixel(rightX, bothY, 0xFF000000 | ~rightColor);
  }
}
