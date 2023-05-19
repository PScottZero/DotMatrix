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
      vramDisplay(256, 192, QImage::Format_RGB32) {
  ui->setupUi(this);
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
    if (cgb->running) {
      for (int vramBank = 0; vramBank < 2; ++vramBank) {
        for (int tile = 0; tile < VRAM_TILE_COUNT; ++tile) {
          uint16 baseAddr = tile < 256 ? TILE_DATA_ADDR_1 : TILE_DATA_ADDR_0;
          uint8 tileNo = tile < 256 ? tile : tile - 256;
          uint16 tileX = (tile % 16) * TILE_PX_DIM;
          uint16 tileY = (tile / 16) * TILE_PX_DIM;
          for (int row = 0; row < TILE_PX_DIM; ++row) {
            auto pixelRow =
                cgb->ppu.getTileRow(baseAddr, tileNo, row, vramBank);
            for (int px = 0; px < TILE_PX_DIM; ++px) {
              auto pixelColor = cgb->ppu.palette->data[pixelRow[px]];
              vramDisplay.setPixel(tileX + px + (vramBank ? 128 : 0),
                                   tileY + row, pixelColor);
            }
          }
        }
      }
    } else {
      vramDisplay.fill(cgb->ppu.palette->data[0]);
    }

    auto pixmap = QPixmap::fromImage(vramDisplay)
                      .scaled(ui->display->width(), ui->display->height(),
                              Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->display->setPixmap(pixmap);
    long long duration = FRAME_DURATION;
    clock += microseconds(duration);
    this_thread::sleep_until(clock);
  }
}
