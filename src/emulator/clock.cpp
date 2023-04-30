#include "clock.h"

Clock::Clock() : clock(system_clock::now()) {}

void Clock::wait(int ns) {
  clock += nanoseconds(ns);
  std::this_thread::sleep_until(clock);
}

void Clock::reset() {
  clock = system_clock::now();
}
