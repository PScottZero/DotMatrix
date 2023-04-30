#pragma once

#include "controls.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

class CGB {
 public:
  Palette *palette;
  Memory mem;
  PPU ppu;
  CPU cpu;
  Timers timers;
  Controls controls;

  CGB(Palette *palette);
  ~CGB();

  void run(QString dir);
  void runFromSaveState();
  void reset();
};
