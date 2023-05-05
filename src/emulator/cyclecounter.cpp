#include "cyclecounter.h"

uint8 CycleCounter::cpuCycles = 0;
uint8 CycleCounter::ppuCycles = 0;

void CycleCounter::addCycles(uint8 count) {
  cpuCycles += count;
  ppuCycles += count;
}
