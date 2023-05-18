// **************************************************
// **************************************************
// **************************************************
// Real Time Clock (RTC) (Used by MBC3)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <array>
#include <chrono>

#include "types.h"

#define RTC_REG_COUNT 5
#define RTC_REG_START_IDX 0x08

#define SECONDS_PER_MINUTE 60ll
#define MINUTES_PER_HOUR 60ll
#define HOURS_PER_DAY 24ll

#define US_PER_SECOND_INT 1000000ll
#define US_PER_MINUTE (SECONDS_PER_MINUTE * US_PER_SECOND_INT)
#define US_PER_HOUR (MINUTES_PER_HOUR * US_PER_MINUTE)
#define US_PER_DAY (HOURS_PER_DAY * US_PER_HOUR)

#define MAX_DAYS 512

using namespace std;
using namespace chrono;

class CGB;

class RTC {
 private:
  uint8 seconds;
  uint8 minutes;
  uint8 hours;
  uint8 daysLo;
  uint8 daysHi;
  uint8 latchVal;
  long long clock;

 public:
  CGB *cgb;
  uint8 *rtcRegs[RTC_REG_COUNT];
  uint8 *rtcReg;

  RTC();

  void latch();
  void resetClock();
  bool halted();
  void load();
  void save();
};
