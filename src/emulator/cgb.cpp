// **************************************************
// **************************************************
// **************************************************
// Game Boy / Game Boy Color
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
      mbc(),
      mem(),
      ppu(),
      apu(),
      timers(),
      rtc(),
      romPath(QDir::currentPath()),
      stop(false),
      cgbMode(true),
      dmgMode(false),
      doubleSpeedMode(false),
      screen(SCREEN_PX_WIDTH, SCREEN_PX_HEIGHT, QImage::Format_RGB32),
      actionPause(nullptr),
      running(false),
      pause(false),
      tempPalette(nullptr) {
  // cgb pointers
  cpu.cgb = this;
  mem.cgb = this;
  ppu.cgb = this;
  apu.cgb = this;
  controls.cgb = this;
  timers.cgb = this;
  rtc.cgb = this;

  // bootstrap
  bootstrap.cgbMode = &cgbMode;

  // mbc
  mbc.mem = &mem;
  mbc.rtc = &rtc;

  // ppu
  ppu.screen = &screen;
  ppu.palette = Palettes::allPalettes[DEFAULT_PALETTE_IDX];
}

CGB::~CGB() {
  running = false;
  wait();

  save();

  std::free(mem.mem);
  std::free(mem.cart);
  std::free(mem.vram);
  std::free(mem.exram);
  std::free(mem.wram);
}

void CGB::run() {
  running = true;
  auto clock = high_resolution_clock::now();
  while (running) {
    if (!pause) {
      cpu.step();

      uint8 cycles = cpu.cpuCycles;
      if (stop) {
        ppu.step();
        cycles = doubleSpeedMode ? 2 : 1;
      }

      if (!bootstrap.skipDmgBootstrap()) {
        int duration = (NS_PER_CYCLE * cycles) / (doubleSpeedMode ? 2 : 1);
        clock += nanoseconds(duration);
        this_thread::sleep_until(clock);
      } else {
        clock = high_resolution_clock::now();
      }
    } else {
      clock = high_resolution_clock::now();
    }
  }
}

bool CGB::loadRom(const QString romPath) {
  mem.loadRom(romPath);

  // get rom config
  mbc.bankType = mem.getByte(BANK_TYPE);
  mbc.romSize = mem.getByte(ROM_SIZE);
  mbc.ramSize = mem.getByte(RAM_SIZE);
  mbc.halfRAMMode = mbc.bankType == MBC2 || mbc.bankType == MBC2_BATTERY;
  dmgMode = !cgbMode;

  // print rom config
  printf("\n>>> Loaded ROM: %s <<<\n", romPath.toStdString().c_str());
  printf("Bank Type: %s (%02X)\n", mbc.bankTypeStr().c_str(), mbc.bankType);
  printf("Has RAM: %s\n", mbc.hasRam() ? "True" : "False");
  printf("Has Battery: %s\n", mbc.hasRamAndBattery() ? "True" : "False");
  printf("Has Timer: %s\n", mbc.hasTimerAndBattery() ? "True" : "False");
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

  // set rom path and load exram and timer
  this->romPath = romPath;
  load();

  return true;
}

// reset game boy
void CGB::reset(bool newGame) {
  // stop current thread and wait for
  // the thread to exit
  running = false;
  wait();

  save();

  // reset flags
  stop = false;
  pause = false;
  doubleSpeedMode = false;
  shouldStepPpuApu = false;
  dmgMode = !cgbMode;
  actionPause->setChecked(false);

  // reset components
  cpu.reset();
  timers.reset();
  mem.reset();
  mbc.reset();
  bootstrap.reset();

  // load external ram if not loading a new game
  // and current game has external ram and
  // a battery
  if (!newGame) load();
}

// set device to either game boy color (cgb)
// or original game boy (dmg)
void CGB::setDevice(bool cgb) {
  cgbMode = cgb;
  restart();
}

// toggle whether boot screen should appear
// before a game is started
void CGB::toggleDmgBootstrap(bool skip) { bootstrap.skipDmg = skip; }

// save current palette and preview
// the specified palette
void CGB::previewPalette(Palette *palette) {
  if (tempPalette == nullptr) tempPalette = ppu.palette;
  ppu.palette = palette;
  renderInPauseMode();
}

// reset palette to original palette
// before palette preview
void CGB::resetPreviewPalette() {
  if (tempPalette != nullptr) {
    ppu.palette = tempPalette;
    tempPalette = nullptr;
  }
  renderInPauseMode();
}

// render screen while in pause mode
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

// toggle pause mode
void CGB::togglePause(bool shouldPause) { pause = shouldPause; }

// restart game boy (reset and start)
void CGB::restart() {
  reset(false);
  if (romPath != QDir::currentPath()) {
    start(QThread::HighestPriority);
  }
}

// save external ram and real-time clock
void CGB::save() {
  if (mbc.hasRamAndBattery()) mem.saveExram();
  if (mbc.hasTimerAndBattery()) rtc.save();
}

// load external ram and real-time clock
void CGB::load() {
  if (mbc.hasRamAndBattery()) mem.loadExram();
  if (mbc.hasTimerAndBattery()) rtc.load();
}
