#pragma once

#include <QWidget>

#include "../emulator/cgb.h"

#define VRAM_TILE_COUNT 384

namespace Ui {
class VramViewer;
}

class VramViewer : public QWidget {
  Q_OBJECT

 public:
  explicit VramViewer(CGB *cgb, QWidget *parent = nullptr);
  ~VramViewer();

  void render();

 private:
  Ui::VramViewer *ui;
  QImage vramDisplay;
  CGB *cgb;
  bool running;
};
