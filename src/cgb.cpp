#include "cgb.h"

#include <chrono>
#include <thread>

using namespace std;

CGB::CGB() : speedMult(1), mem(), ppu(mem, speedMult), cpu(mem, speedMult) {}

void CGB::run() {
  thread cpuThread(&CPU::run, cpu);
  thread ppuThread(&PPU::run, ppu);
  cpuThread.detach();
  ppuThread.detach();
}
