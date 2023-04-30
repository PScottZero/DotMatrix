#pragma once

#include "controls.h"
#include "cpu.h"
#include "interrupts.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

class CGB {
 public:
  float speedMult;
  bool stop;
  bool threadsRunning;
  bool bootstrapMode;
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
};
