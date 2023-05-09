// **************************************************
// **************************************************
// **************************************************
// CYCLE COUNTER (KEEPS TRACK OF CPU + PPU CYCLES)
// **************************************************
// **************************************************
// **************************************************

#include "cyclecounter.h"

uint8 CycleCounter::cpuCycles = 0;
uint16 CycleCounter::ppuCycles = 0;
uint16 CycleCounter::serialTransferCycles = 0;
bool CycleCounter::serialTransferMode = false;

void CycleCounter::addCycles(uint8 count) {
  cpuCycles += count;
  ppuCycles += count;
  if (serialTransferMode) serialTransferCycles += count;
}

bool CycleCounter::serialTransferComplete() {
  bool completed = serialTransferCycles >= SERIAL_TRANSFER_CYCLES;
  if (completed) {
    serialTransferCycles = 0;
    serialTransferMode = false;
  }
  return completed;
}
