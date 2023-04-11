#pragma once

#include "cpu.h"
#include "memory.h"
#include "ppu.h"

class CGB {
 private:
  int speedMult;
  Memory mem;
  PPU ppu;
  CPU cpu;

 public:
  CGB();

  void run();
};
