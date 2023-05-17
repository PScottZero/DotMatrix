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
#include "interrupts.h"
#include "mbc.h"
#include "memory.h"
#include "ppu.h"
#include "timers.h"

using namespace std;
using namespace chrono;

CGB::CGB()
    : bootstrap(),
      controls(),
      cpu(),
      interrupts(),
      mbc(),
      mem(),
      ppu(),
      timers(),
      romPath(QDir::currentPath()),
      stop(false),
      dmgMode(false),
      doubleSpeedMode(false),
      shouldStepPpu(false),
      screen(SCREEN_PX_WIDTH, SCREEN_PX_HEIGHT, QImage::Format_RGB32),
      actionPause(nullptr),
      running(false),
      pause(false),
      tempPalette(nullptr) {
  init();
}

CGB::~CGB() {
  running = false;
  wait();

  if (mbc.hasRamAndBattery()) mem.saveExram();

  std::free(mem.mem);
  std::free(mem.cart);
  std::free(mem.vram);
  std::free(mem.exram);
  std::free(mem.wram);
}

void CGB::run() {
  running = true;
  auto clock = system_clock::now();
  while (running) {
    if (!pause) {
      cpu.step();
      if (stop) ppu.step();

      // send rendered screen frame to ui
      if (!bootstrap.enabledAndShouldSkip()) {
        if (ppu.frameRendered) {
          emit sendScreen(&screen);
          int duration = FRAME_DURATION;
          clock += microseconds(duration);
          this_thread::sleep_until(clock);
          ppu.frameRendered = false;
        }
      } else {
        clock = system_clock::now();
      }
    } else {
      clock = system_clock::now();
    }
  }
}

// initialize memory pointers between
// components of game boy
void CGB::init() {
  // cgb pointers
  cpu.cgb = this;
  mem.cgb = this;
  ppu.cgb = this;
  timers.cgb = this;

  // boostrap
  bootstrap.dmgMode = &dmgMode;

  // controls
  controls.interrupts = &interrupts;
  controls.p1 = mem.getBytePtr(P1);

  // interrupts
  interrupts.intEnable = mem.getBytePtr(IE);
  interrupts.intFlags = mem.getBytePtr(IF);

  // mbc
  mbc.mem = &mem;

  // ppu
  ppu.screen = &screen;
  ppu.palette = Palettes::allPalettes[DEFAULT_PALETTE_IDX];
  ppu.lcdc = mem.getBytePtr(LCDC);
  ppu.stat = mem.getBytePtr(STAT);
  ppu.scy = mem.getBytePtr(SCY);
  ppu.scx = mem.getBytePtr(SCX);
  ppu.ly = mem.getBytePtr(LY);
  ppu.lyc = mem.getBytePtr(LYC);
  ppu.bgp = mem.getBytePtr(BGP);
  ppu.obp0 = mem.getBytePtr(OBP0);
  ppu.obp1 = mem.getBytePtr(OBP1);
  ppu.wx = mem.getBytePtr(WX);
  ppu.wy = mem.getBytePtr(WY);

  // timers
  timers.div = mem.getBytePtr(DIV);
  timers.tima = mem.getBytePtr(TIMA);
  timers.tma = mem.getBytePtr(TMA);
  timers.tac = mem.getBytePtr(TAC);
}

bool CGB::loadRom(const QString romPath) {
  mem.loadRom(romPath);

  // get rom config
  mbc.bankType = mem.getByte(BANK_TYPE);
  mbc.romSize = mem.getByte(ROM_SIZE);
  mbc.ramSize = mem.getByte(RAM_SIZE);
  mbc.halfRAMMode = mbc.bankType == MBC2_ || mbc.bankType == MBC2_BATTERY;

  // print rom config
  printf("\n>>> Loaded ROM: %s <<<\n", romPath.toStdString().c_str());
  printf("Game Boy Mode: %s\n", dmgMode ? "DMG" : "CGB");
  printf("Bank Type: %s (%02X)\n", mbc.bankTypeStr().c_str(), mbc.bankType);
  printf("Has RAM: %s\n", mbc.hasRam() ? "true" : "false");
  printf("Has Battery: %s\n", mbc.hasRamAndBattery() ? "true" : "false");
  printf("ROM Size: %d KiB\n", (int)pow(2, mbc.romSize + 1) * ROM_BANK_BYTES);
  printf("RAM Size: %d KiB\n", mbc.ramBytes());

  // check if mbc type of cartridge
  // is implemented
  if (!mbc.bankTypeImplemented()) {
    QMessageBox mbox{};
    auto message = "Bank type " + mbc.bankTypeStr() + " is not supported";
    mbox.setText(QString::fromStdString(message));
    mbox.exec();
    return false;
  }

  // set rom path and load exram
  this->romPath = romPath;
  if (mbc.hasRamAndBattery()) mem.loadExram();

  return true;
}

void CGB::reset(bool newGame) {
  running = false;
  wait();
  stop = false;
  pause = false;
  doubleSpeedMode = false;
  shouldStepPpu = false;
  actionPause->setChecked(false);

  cpu.reset();
  timers.reset();
  mem.reset();
  mbc.reset();
  bootstrap.reset();

  if (!newGame && mbc.hasRamAndBattery()) {
    mem.loadExram();
  }
}

void CGB::previewPalette(Palette *palette) {
  if (dmgMode) {
    if (tempPalette == nullptr) tempPalette = ppu.palette;
    ppu.palette = palette;
    renderInPauseMode();
  }
}

void CGB::resetPreviewPalette() {
  if (dmgMode) {
    if (tempPalette != nullptr) {
      ppu.palette = tempPalette;
      tempPalette = nullptr;
    }
    renderInPauseMode();
  }
}

void CGB::renderInPauseMode() {
  if (pause) {
    ppu.cycles = 0;
    for (int i = 0; i < SCANLINE_CYCLES * SCREEN_LINES; ++i) {
      ppu.step();
    }
    ppu.frameRendered = false;
    emit sendScreen(&screen);
  }
}
