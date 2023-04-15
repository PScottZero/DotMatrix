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
  cpu.start();
  ppu.start();
}

void CGB::loadROM(QString dir) { mem.loadROM(dir); }
