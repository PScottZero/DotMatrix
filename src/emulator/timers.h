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

class Timers {
 private:
  static const uint16 internalCounterMasks[4];

  static bool timerEnabled();
  static uint8 timerFreq();

 public:
  static uint8 *div, *tima, *tma, *tac;
  static uint16 internalCounter;

  static void step();
  static void reset();
};
