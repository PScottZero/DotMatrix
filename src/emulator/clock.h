#pragma once

#include <QThread>
#include <chrono>
#include <thread>

using namespace std::chrono;

#define CLOCK_NORMAL_SPEED 1
#define CLOCK_FAST_FORWARD 100

#define DMG_CLOCK_SPEED 0x100000  // 1MHz
#define NS_PER_SEC 1000000000
#define CYCLE_TIME_NS NS_PER_SEC / DMG_CLOCK_SPEED

enum ClockType { CPU_CLOCK, PPU_CLOCK, TIMERS_CLOCK };

typedef time_point<system_clock, nanoseconds> timepoint;

class Clock {
 private:
  static timepoint cpuClock;
  static timepoint ppuClock;
  static timepoint timersClock;

 public:
  static float speedMult;
  static bool threadsRunning;
  static bool stop;

  static void wait(ClockType type, int cycles);
  static void reset();
};
