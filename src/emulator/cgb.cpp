#include "cgb.h"

#include <math.h>

#include <QDir>
#include <QMessageBox>

#include "bootstrap.h"
#include "controls.h"
#include "cpu.h"
#include "mbc.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

using namespace std;
using namespace chrono;

QString CGB::romPath = QDir::currentPath();
bool CGB::stop = false;

CGB::CGB()
    : screen(SCREEN_PX_WIDTH, SCREEN_PX_HEIGHT, QImage::Format_RGB32),
      actionPause(nullptr),
      pause(false),
      speedMult(1.0) {
  PPU::screen = &screen;
  PPU::palette = Palettes::allPalettes[DEFAULT_PALETTE_IDX];
}

CGB::~CGB() {
  terminate();
  wait();

  if (MBC::hasRam()) Memory::saveExram();

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
    if (!pause) {
      Controls::update();
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
}

bool CGB::loadRom(const QString romPath) {
  Memory::loadRom(romPath);

  MBC::bankType = Memory::getByte(BANK_TYPE);
  MBC::romSize = Memory::getByte(ROM_SIZE);
  MBC::ramSize = Memory::getByte(RAM_SIZE);
  MBC::halfRAMMode = MBC::bankType == MBC2_ || MBC::bankType == MBC2_BATTERY;

  printf("\n>>> Loaded ROM: %s <<<\n", romPath.toStdString().c_str());
  printf("Bank Type: %s\n", MBC::bankTypeStr().c_str());
  printf("ROM Size: %06X\n", (int)pow(2, MBC::romSize + 1) * ROM_BANK_BYTES);
  printf("RAM Size: %05X\n\n", MBC::ramBytes());

  if (!MBC::bankTypeImplemented()) {
    QMessageBox mbox{};
    auto message = "Bank type " + MBC::bankTypeStr() + " is not supported";
    mbox.setText(QString::fromStdString(message));
    mbox.exec();
    return false;
  }

  CGB::romPath = romPath;
  if (MBC::hasRam()) Memory::loadExram();

  return true;
}

void CGB::reset() {
  terminate();
  wait();
  stop = false;
  pause = false;
  actionPause->setChecked(false);

  if (MBC::hasRam()) Memory::saveExram();

  CPU::reset();
  Timers::reset();
  Memory::reset();
  MBC::reset();
  Bootstrap::reset();
}
