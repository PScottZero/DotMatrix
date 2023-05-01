#pragma once

#include <QThread>
#include <chrono>
#include <thread>

#define CLOCK_NORMAL_SPEED 1
#define CLOCK_FAST_FORWARD 0.5

#define DMG_CLOCK_SPEED 0x100000  // 1MHz
#define NS_PER_SEC 1000000000
#define CYCLE_TIME_NS NS_PER_SEC / DMG_CLOCK_SPEED

using namespace std::chrono;

enum ClockType { CPU_CLOCK, PPU_CLOCK, TIMERS_CLOCK };

typedef time_point<system_clock, nanoseconds> timepoint;

class Clock {
 private:
  static timepoint cpuClock;
  static timepoint ppuClock;
  static timepoint timersClock;

 public:
  static float speedMult;
  static bool stop;
  static bool threadsRunning;

  static void wait(ClockType type, int cycles);
  static void reset();
};
