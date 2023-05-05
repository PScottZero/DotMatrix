#pragma once

#include "types.h"

class CycleCounter {
 public:
  static uint8 cpuCycles;
  static uint8 ppuCycles;

  static void addCycles(uint8 count);
};
