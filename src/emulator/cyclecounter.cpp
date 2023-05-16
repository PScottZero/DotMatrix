// **************************************************
// **************************************************
// **************************************************
// CYCLE COUNTER (KEEPS TRACK OF CPU + PPU CYCLES)
// **************************************************
// **************************************************
// **************************************************

#include "cyclecounter.h"

#include "ppu.h"
#include "timers.h"

uint16 CycleCounter::serialTransferCycles = 0;
bool CycleCounter::serialTransferMode = false;

void CycleCounter::addCycles(uint8 count) {
  for (int i = 0; i < count; ++i) {
    PPU::step();
    Timers::step();
  }
}
