#include "vramviewer.h"

#include <chrono>

#include "ui_vramviewer.h"

using namespace chrono;

VramViewer::VramViewer(CGB *cgb, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::VramViewer),
      cgb(cgb),
      vramBank(false),
      running(true),
      vramDisplay(128, 192, QImage::Format_RGB32),
      renderThread(&VramViewer::render, this) {
  renderThread.detach();

  ui->setupUi(this);

  if (cgb->dmgMode) ui->bank1Button->setEnabled(false);
  connect(ui->bank0Button, &QPushButton::clicked, this,
          [this] { setVramBank(false); });
  connect(ui->bank1Button, &QPushButton::clicked, this,
          [this] { setVramBank(true); });
  connect(ui->pauseButton, &QPushButton::clicked, this,
          [this, cgb] { cgb->pause = !cgb->pause; });
}

VramViewer::~VramViewer() {
  running = false;
  delete ui;
}

void VramViewer::render() {
  auto clock = system_clock::now();
  while (running) {
    for (int tile = 0; tile < VRAM_TILE_COUNT; ++tile) {
      uint16 baseAddr = tile < 256 ? BG_DATA_ADDR_1 : BG_DATA_ADDR_0;
      uint8 tileNo = tile < 256 ? tile : tile - 256;
      uint16 tileX = (tile % 16) * TILE_PX_DIM;
      uint16 tileY = (tile / 16) * TILE_PX_DIM;
      for (int row = 0; row < TILE_PX_DIM; ++row) {
        auto pixelRow = cgb->ppu.getTileRow(baseAddr, tileNo, row, vramBank);
        for (int px = 0; px < TILE_PX_DIM; ++px) {
          auto pixelColor = cgb->ppu.palette->data[pixelRow[px]];
          vramDisplay.setPixel(tileX + px, tileY + row, pixelColor);
        }
      }
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

void VramViewer::setVramBank(bool bank) { vramBank = bank; }
