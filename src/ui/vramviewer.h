#pragma once

#include <QDialog>
#include <thread>

#include "../emulator/cgb.h"

#define VRAM_TILE_COUNT 384

namespace Ui {
class VramViewer;
}

class VramViewer : public QDialog {
  Q_OBJECT

 public:
  explicit VramViewer(CGB *cgb, QWidget *parent = nullptr);
  ~VramViewer();

  void render();

 private:
  Ui::VramViewer *ui;
  QImage vramDisplay;
  CGB *cgb;
  bool vramBank, running;
  std::thread renderThread;

 public slots:
  void setVramBank(bool bank);
};
