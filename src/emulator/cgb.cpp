#include "cgb.h"

#include <QMessageBox>

#include "bootstrap.h"
#include "cpu.h"
#include "mbc/mbc.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

using namespace std;
using namespace chrono;

bool CGB::stop = false;
float CGB::speedMult = 1.0;

CGB::CGB() : screen(SCREEN_PX_WIDTH, SCREEN_PX_HEIGHT, QImage::Format_RGB32) {
  PPU::screen = &screen;
}

CGB::~CGB() {
  terminate();
  wait();

  free(Memory::mem);
  free(Memory::cart);
  free(Memory::exram);
  free(Memory::romBank0);
  free(Memory::romBank1);
  free(Memory::exramBank);
}

void CGB::run() {
  auto clock = system_clock::now();
  while (isRunning()) {
    CPU::step();
    Timers::step();
    PPU::step();

    if (!Bootstrap::skipWait()) {
      if (PPU::frameRendered) {
        emit sendScreen(screen);
        clock += microseconds((int)(FRAME_DURATION / CGB::speedMult));
        this_thread::sleep_until(clock);
        PPU::frameRendered = false;
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
  terminate();
  wait();
  stop = false;
  CPU::reset();
  Timers::reset();
  Memory::reset();
  MBC::reset();
  Bootstrap::reset();
}
