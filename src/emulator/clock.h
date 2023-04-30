#pragma once

#include <chrono>
#include <thread>

using namespace std::chrono;

class Clock {
  private:
    time_point<system_clock, nanoseconds> clock;
  
  public:
    Clock();

    void wait(int ns);
    void reset();
};
