#include "cgb.h"

#include <QMessageBox>

#include "bootstrap.h"
#include "controls.h"
#include "interrupts.h"
#include "mbc.h"
#include "timers.h"

using namespace std;
using namespace chrono;

CGB::CGB(Palette *palette)
    : running(false),
      stop(false),
      speedMult(1),
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
  running = false;
  wait();

  free(Memory::mem);
  free(Memory::cart);
  free(Memory::exram);
  free(Memory::romBank0);
  free(Memory::romBank1);
  free(Memory::exramBank);
}

void CGB::run() {
  running = true;
  auto clock = system_clock::now();
  while (running) {
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

bool CGB::loadROM(const QString dir) {
  Memory::loadROM(dir);
  MBC::bankType = Memory::getByte(BANK_TYPE);
  MBC::cartSize = Memory::getByte(CART_SIZE);
  if (!MBC::bankTypeImplemented()) {
    QMessageBox mbox{};
    auto message = "Bank type " + MBC::bankTypeStr() + " is not supported";
    mbox.setText(QString::fromStdString(message));
    mbox.exec();
    return false;
  }
  return true;
}

void CGB::reset() {
  running = false;
  wait();
  cpu.reset();
  Memory::reset();
  Timers::internalCounter = 0;
}
