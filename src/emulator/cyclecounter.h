// **************************************************
// **************************************************
// **************************************************
// CYCLE COUNTER (KEEPS TRACK OF CPU + PPU CYCLES)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include "types.h"

#define CPU_CLOCK_SPEED 0x100000
#define SERIAL_TRANSFER_SPEED 0x2000
#define SERIAL_TRANSFER_CYCLES (CPU_CLOCK_SPEED / SERIAL_TRANSFER_SPEED) * 8

class CycleCounter {
 public:
  static uint8 cpuCycles;
  static uint16 ppuCycles, serialTransferCycles;
  static bool serialTransferMode;

  static void addCycles(uint8 count);

  static bool serialTransferComplete();
};
