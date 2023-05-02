#pragma once

#include <QThread>

#include "controls.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

#define FRAME_RATE 59.7275
#define US_PER_SEC 1000000.0
#define FRAME_DURATION US_PER_SEC / 59.7275

class CGB : public QThread {
  Q_OBJECT

 public:
  bool stop;
  float speedMult;
  Palette *palette;
  Memory mem;
  PPU ppu;
  CPU cpu;

  CGB(Palette *palette);
  ~CGB();

  void run() override;
  void loadROM(const QString dir);
  void reset();

 signals:
  void sendScreen(QImage);
};
