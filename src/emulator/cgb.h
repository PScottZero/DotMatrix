// **************************************************
// **************************************************
// **************************************************
// Game Boy / Game Boy Color
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QAction>
#include <QImage>
#include <QThread>

#include "../ui/palettes.h"
#include "bootstrap.h"
#include "controls.h"
#include "cpu.h"
#include "mbc.h"
#include "memory.h"
#include "ppu.h"
#include "rtc.h"
#include "timers.h"

#define FRAME_RATE 59.7275
#define US_PER_SEC 1000000.0
#define FRAME_DURATION US_PER_SEC / 59.7275

class CGB : public QThread {
  Q_OBJECT

 private:
  QImage screen;

 public:
  Bootstrap bootstrap;
  Controls controls;
  CPU cpu;
  MBC mbc;
  Memory mem;
  PPU ppu;
  Timers timers;
  RTC rtc;

  QString romPath;
  QAction *actionPause;
  bool stop, cgbMode, dmgMode, doubleSpeedMode, shouldStepPpu;
  bool running, pause;
  Palette *tempPalette;

  CGB();
  ~CGB();

  void run() override;
  void reset(bool newGame = true);
  bool loadRom(const QString romPath);
  void renderInPauseMode();

 signals:
  void sendScreen(const QImage *screen);

 public slots:
  void setDevice(bool cgb);
  void toggleDmgBootstrap(bool skip);
  void previewPalette(Palette *palette);
  void resetPreviewPalette();
  void togglePause(bool shouldPause);
  void restart();
  void save();
  void load();
};
