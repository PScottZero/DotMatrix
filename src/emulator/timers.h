#pragma once

#include <QThread>
#include <chrono>

#include "memory.h"
#include "types.h"

#define TIMER_BASE_FREQ 0x100000  // 1MHz
#define TIMA_CYCLES_00 256
#define TIMA_CYCLES_01 4
#define TIMA_CYCLES_10 16
#define TIMA_CYCLES_11 64
#define DIV_CYCLES 64

class Timers : public QThread {
  Q_OBJECT

 private:
  uint8 &div, &tima, &tma, &tac;

  int timaFreqs[4];
  int timaCycles;
  int divCycles;

  void updateTimers();
  bool timerEnabled();
  uint8 timerFreq();

 public:
  Timers(Memory &mem);
  ~Timers();

  void run() override;
};
