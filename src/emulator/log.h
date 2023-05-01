#pragma once

#include <fstream>
#include <map>

#include "interrupts.h"

#define LINE_BYTES 256

class Log {
 private:
  static std::fstream cpuLog;
  static std::fstream intLog;
  static const std::map<uint8, std::string> intName;

 public:
  static void logCPUState(uint16 PC, uint16 SP, uint8 A, uint16 BC, uint16 DE,
                          uint16 HL, uint8 lcdc, uint8 stat, uint8 ly,
                          uint8 scx, uint8 scy, bool IME, uint8 intEnable,
                          uint8 intFlags, bool carry, bool halfCarry,
                          bool subtract, bool zero);
  static void logInterruptRequest(uint8 interrupt);
  static void logInterruptService(uint16 PC, uint8 interrupt);
  static void logInterruptReturn(uint16 PC, uint8 interrupt);
};
