#pragma once

#include <QWidget>

#include "../emulator/cgb.h"

#define VRAM_TILE_COUNT 384
#define VIEWER_TILE_ROWS 24
#define VIEWER_TILES_PER_ROW 32

#define TILES_TAB 0
#define BG_TAB 1
#define WIN_TAB 2

namespace Ui {
class VramViewer;
}

class VramViewer : public QWidget {
  Q_OBJECT

 public:
  explicit VramViewer(CGB *cgb, QWidget *parent = nullptr);
  ~VramViewer();

  void render();
  void renderTiles();
  void renderBackground();
  void renderWindow();
  void renderTileMap(QImage &display, uint16 tileMapAddr);
  void renderBgViewport();
  void renderWinViewport();

 private:
  Ui::VramViewer *ui;
  QImage vramDisplay, bgDisplay, winDisplay;
  CGB *cgb;
  bool running;
};
