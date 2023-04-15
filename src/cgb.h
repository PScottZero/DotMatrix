#pragma once

#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "utils.h"

class CGB {
 public:
  float speedMult;
  Palette *palette;
  Memory mem;
  PPU ppu;
  CPU cpu;
  
  CGB(Palette *palette);

  void run();
  void loadROM(QString dir);
};
