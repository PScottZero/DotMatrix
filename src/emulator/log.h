#pragma once

#include <fstream>
#include <map>
#include <mutex>
#include <thread>

#include "clock.h"
#include "types.h"

#define LINE_BYTES 256

using namespace std;

class Log {
 private:
  static fstream cpuLog;
  static fstream intLog;
  static fstream clockLog;
  static mutex cpuLogMutex;
  static mutex intLogMutex;
  static mutex clockLogMutex;
  static const map<uint8, string> intName;
  static const map<ClockType, string> clockName;

 public:
  static void logCPUState(uint16 PC, uint16 SP, uint8 A, uint16 BC, uint16 DE,
                          uint16 HL, uint8 lcdc, uint8 stat, uint8 ly,
                          uint8 scx, uint8 scy, bool IME, uint8 intEnable,
                          uint8 intFlags, bool carry, bool halfCarry,
                          bool subtract, bool zero);
  static void logInterruptEnable(uint16 PC);
  static void logInterruptDisable(uint16 PC);
  static void logInterruptRequest(uint8 interrupt);
  static void logInterruptService(uint16 PC, uint8 interrupt);
  static void logInterruptReturn(uint16 PC);
  static void logClock(ClockType type, timepoint time);
};
