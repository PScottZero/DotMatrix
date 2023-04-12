#pragma once

#include "cpu.h"
#include "utils.h"
#include "memory.h"
#include "ppu.h"

class CGB {
private:
  Memory mem;
  PPU ppu;
  CPU cpu;

 public:
  float speedMult;
  Palette *palette;

  CGB(Palette *palette);

  void run();
};
