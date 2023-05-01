#include "cgb.h"

#include "clock.h"
#include "interrupts.h"

CGB::CGB(Palette *palette)
    : palette(palette),
      mem(),
      ppu(mem, palette),
      cpu(mem),
      timers(mem),
      controls(mem) {
  mem.controls = &controls;
  Interrupts::intEnable = mem.getBytePtr(IE);
  Interrupts::intFlags = mem.getBytePtr(IF);
}

CGB::~CGB() {
  Clock::threadsRunning = false;
  cpu.wait();
  ppu.wait();
  timers.wait();
}

void CGB::run(QString dir) {
  mem.loadROM(dir);
  Clock::reset();
  cpu.start();
  ppu.start();
  timers.start();
}

void CGB::runFromSaveState() {
  cpu.loadState();
  mem.loadState();
  Clock::reset();
  cpu.start();
  ppu.start();
  timers.start();
}

void CGB::reset() {}
