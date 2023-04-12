#include "cgb.h"

using namespace std;

CGB::CGB(Palette *palette)
    : speedMult(1.0),
      palette(palette),
      mem(),
      ppu(mem, palette, speedMult),
      cpu(mem, speedMult) {}

void CGB::run() {
  mem.init();
  thread cpuThread(&CPU::run, cpu);
  thread ppuThread(&PPU::run, ppu);
  cpuThread.detach();
  ppuThread.detach();
}
