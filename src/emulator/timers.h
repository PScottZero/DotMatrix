// **************************************************
// **************************************************
// **************************************************
// DIV & TIMA Timers
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QThread>
#include <chrono>

#include "types.h"

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
  uint16 internalCounter;

  Timers();

  void step();
  void reset();
};
