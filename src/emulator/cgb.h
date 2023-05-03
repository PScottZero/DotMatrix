#pragma once

#include <QThread>

#include "cpu.h"
#include "memory.h"
#include "ppu.h"

#define FRAME_RATE 59.7275
#define US_PER_SEC 1000000.0
#define FRAME_DURATION US_PER_SEC / 59.7275

class CGB : public QThread {
  Q_OBJECT

 public:
  bool running;
  bool stop;
  float speedMult;
  Memory mem;
  PPU ppu;
  CPU cpu;

  CGB(Palette *palette);
  ~CGB();

  void run() override;
  bool loadROM(const QString dir);
  void reset();

 signals:
  void sendScreen(QImage);
};
