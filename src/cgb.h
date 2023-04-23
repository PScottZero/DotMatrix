#pragma once

#include "controls.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

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
  Controls controls;

  CGB(Palette *palette);
  ~CGB();

  void run(QString dir);
};
