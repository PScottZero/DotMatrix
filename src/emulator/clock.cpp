#include "clock.h"

#include "bootstrap.h"

timepoint Clock::cpuClock = system_clock::now();
timepoint Clock::ppuClock = Clock::cpuClock + nanoseconds(0);
timepoint Clock::timersClock = Clock::cpuClock + nanoseconds(0);

float Clock::speedMult = CLOCK_NORMAL_SPEED;
bool Clock::threadsRunning = false;
bool Clock::stop = false;

void Clock::wait(ClockType type, int cycles) {
  timepoint *clock = nullptr;
  switch (type) {
    case ClockType::CPU_CLOCK:
      clock = &Clock::cpuClock;
      break;
    case ClockType::PPU_CLOCK:
      clock = &Clock::ppuClock;
      break;
    case ClockType::TIMERS_CLOCK:
      clock = &Clock::timersClock;
      break;
  }

  if (clock != nullptr) {
    float speedMult =
        Bootstrap::enabled ? Bootstrap::speedMult : Clock::speedMult;
    *clock += nanoseconds((int)(CYCLE_TIME_NS * cycles / speedMult));
    std::this_thread::sleep_until(*clock);
  }
}

void Clock::reset() {
  Clock::cpuClock = system_clock::now();
  Clock::ppuClock = Clock::cpuClock + nanoseconds(0);
  Clock::timersClock = Clock::cpuClock + nanoseconds(0);
}
