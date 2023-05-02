#include "cgb.h"

#include "bootstrap.h"
#include "interrupts.h"

using namespace std;
using namespace chrono;

CGB::CGB(Palette *palette)
    : stop(false),
      speedMult(1),
      palette(palette),
      mem(),
      ppu(mem, palette),
      cpu(mem, stop) {
  Interrupts::intEnable = mem.getBytePtr(IE);
  Interrupts::intFlags = mem.getBytePtr(IF);
  Controls::p1 = mem.getBytePtr(P1);
  Timers::div = mem.getBytePtr(DIV);
  Timers::tima = mem.getBytePtr(TIMA);
  Timers::tma = mem.getBytePtr(TMA);
  Timers::tac = mem.getBytePtr(TAC);
}

CGB::~CGB() {
  terminate();
  wait();
}

void CGB::run() {
  auto clock = system_clock::now();
  while (isRunning()) {
    uint8 cycles = cpu.step();
    ppu.step(cycles);
    Timers::step(cycles);

    if (!Bootstrap::skipWait()) {
      if (ppu.frameRendered) {
        emit sendScreen(ppu.screen);
        int frameDuration = FRAME_DURATION / speedMult;
        clock += microseconds(frameDuration);
        this_thread::sleep_until(clock);
        ppu.frameRendered = false;
      }
    } else {
      clock = system_clock::now();
    }
  }
}

void CGB::loadROM(const QString dir) { mem.loadROM(dir); }

void CGB::reset() {
  terminate();
  wait();
  cpu.reset();
  mem.reset();
  Timers::internalCounter = 0;
}
