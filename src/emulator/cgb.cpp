#include "cgb.h"

CGB::CGB(Palette *palette)
    : speedMult(1.0),
      stop(false),
      threadsRunning(false),
      bootstrapMode(true),
      palette(palette),
      mem(bootstrapMode),
      ppu(mem, palette, speedMult, stop, threadsRunning, bootstrapMode),
      cpu(mem, speedMult, stop, threadsRunning, bootstrapMode),
      timers(mem, speedMult, stop, threadsRunning),
      controls(mem) {
  mem.controls = &controls;
  Interrupts::intEnable = mem.getBytePtr(IE);
  Interrupts::intFlags = mem.getBytePtr(IF);
}

CGB::~CGB() {
  threadsRunning = false;
  cpu.wait();
  ppu.wait();
  timers.wait();
}

void CGB::run(QString dir) {
  mem.loadROM(dir);
  threadsRunning = true;
  cpu.start();
  ppu.start();
  timers.start();
}

void CGB::runFromSaveState() {
  cpu.loadState();
  mem.loadState();
  threadsRunning = true;
  cpu.start();
  ppu.start();
  timers.start();
}
