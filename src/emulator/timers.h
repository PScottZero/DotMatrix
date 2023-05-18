// **************************************************
// **************************************************
// **************************************************
// TIMERS CONTROLLER (DIV, TIMA)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QThread>
#include <chrono>

#include "types.h"

#define TIMER_BASE_FREQ 0x100000  // 1MHz
#define TAC_00 0x03FF
#define TAC_01 0x000F
#define TAC_10 0x003F
#define TAC_11 0x00FF

#define DIV_MASK 0xFF00

class CGB;

class Timers {
 private:
  static const uint16 internalCounterMasks[4];
  bool timaOverflow;

  bool timerEnabled() const;
  uint8 timerFreq() const;

 public:
  CGB *cgb;
  uint8 *div, *tima, *tma, *tac;
  uint16 internalCounter;

  Timers();

  void step();
  void reset();
};
