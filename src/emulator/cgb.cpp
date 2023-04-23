#include "cgb.h"

CGB::CGB(Palette *palette)
    : speedMult(1.0),
      stop(false),
      threadsRunning(false),
      palette(palette),
      mem(),
      timers(cpu, mem, speedMult, stop, threadsRunning),
      ppu(mem, palette, speedMult, stop, threadsRunning),
      cpu(mem, speedMult, stop, threadsRunning),
      controls(mem) {
  mem.controls = &controls;
}

CGB::~CGB() {
  threadsRunning = false;
  cpu.wait();
  ppu.wait();
  timers.wait();
}

void CGB::run(QString dir) {
  mem.init();
  mem.loadROM(dir);
  threadsRunning = true;
  cpu.start();
  ppu.start();
  timers.start();
}
