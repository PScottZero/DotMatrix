// **************************************************
// **************************************************
// **************************************************
// GAME BOY COLOR (CURRENTLY ONLY SUPPORTS DMG)
// **************************************************
// **************************************************
// **************************************************

#include "cgb.h"

#include <math.h>

#include <QDir>
#include <QMessageBox>

#include "bootstrap.h"
#include "controls.h"
#include "cpu.h"
#include "cyclecounter.h"
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
      running(false),
      pause(false) {
  PPU::screen = &screen;
  PPU::palette = Palettes::allPalettes[DEFAULT_PALETTE_IDX];
}

CGB::~CGB() {
  running = false;
  wait();

  if (MBC::hasRamAndBattery()) Memory::saveExram();

  std::free(Memory::mem);
  std::free(Memory::cart);
  std::free(Memory::exram);
}

void CGB::run() {
  running = true;
  auto clock = system_clock::now();
  while (running) {
    if (!pause) {
      Controls::update();
      CPU::step();
      Timers::step();
      if (stop) CycleCounter::ppuCycles += 1;
      PPU::step();

      // send rendered screen frame to ui
      if (!Bootstrap::enabledAndShouldSkip()) {
        if (PPU::frameRendered) {
          emit sendScreen(screen);
          int duration = FRAME_DURATION;
          clock += microseconds(duration);
          this_thread::sleep_until(clock);
          PPU::frameRendered = false;
        }
      } else {
        clock = system_clock::now();
      }
    } else {
      clock = system_clock::now();
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
  printf("Bank Type: %s (%02X)\n", MBC::bankTypeStr().c_str(), MBC::bankType);
  printf("Has RAM: %s\n", MBC::hasRam() ? "true" : "false");
  printf("Has Battery: %s\n", MBC::hasRamAndBattery() ? "true" : "false");
  printf("ROM Size: %d KiB\n", (int)pow(2, MBC::romSize + 1) * ROM_BANK_BYTES);
  printf("RAM Size: %d KiB\n", MBC::ramBytes());

  if (!MBC::bankTypeImplemented()) {
    QMessageBox mbox{};
    auto message = "Bank type " + MBC::bankTypeStr() + " is not supported";
    mbox.setText(QString::fromStdString(message));
    mbox.exec();
    return false;
  }

  CGB::romPath = romPath;
  if (MBC::hasRamAndBattery()) Memory::loadExram();

  return true;
}

void CGB::reset(bool newGame) {
  running = false;
  wait();
  stop = false;
  pause = false;
  actionPause->setChecked(false);

  CPU::reset();
  Timers::reset();
  Memory::reset();
  MBC::reset();
  Bootstrap::reset();

  if (!newGame && MBC::hasRamAndBattery()) {
    Memory::loadExram();
  }
}
