#pragma once

#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"
#include "utils.h"

class CGB {
 public:
  float speedMult;
  bool stop;
  bool threadsRunning;
  Palette *palette;
  Memory mem;
  Timers timers;
  PPU ppu;
  CPU cpu;

  CGB(Palette *palette);
  ~CGB();

  void run(QString dir);
};
