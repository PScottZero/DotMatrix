#pragma once

#include <fstream>
#include <map>
#include <thread>

#include "types.h"

#define LINE_BYTES 256

using namespace std;

class Log {
 private:
  static fstream log;
  static const map<uint8, string> intName;

 public:
  static void logCPUState(uint16 PC, uint16 SP, uint8 A, uint16 BC, uint16 DE,
                          uint16 HL, bool carry, bool halfCarry, bool subtract,
                          bool zero, bool IME, uint8 intEnable, uint8 intFlags,
                          uint8 lcdc, uint8 stat, uint8 ly);
  static void logCPUCycles(uint8 opcode, uint8 cbOpcode, uint8 cycles);
  static void logInterruptEnable(uint16 PC);
  static void logInterruptDisable(uint16 PC);
  static void logInterruptRequest(uint8 interrupt);
  static void logInterruptService(uint16 PC, uint8 interrupt);
  static void logInterruptReturn(uint16 PC);
};
