// **************************************************
// **************************************************
// **************************************************
// LOGGER
// **************************************************
// **************************************************
// **************************************************

#include "log.h"

#include <string.h>

#include "bootstrap.h"
#include "interrupts.h"

// fstream Log::log("/home/paul/git/DotMatrix/debug/log.txt", ios::out);
fstream Log::log("/Users/paulscott/git/DotMatrix/debug/log.txt", ios::out);

bool Log::enable = false;

const map<uint8, string> Log::intName = {
    {V_BLANK_INT, "VBLANK"}, {LCDC_INT, "LCDC"},     {TIMER_INT, "TIMER"},
    {SERIAL_INT, "SERIAL"},  {JOYPAD_INT, "JOYPAD"},
};

void Log::logCPUState(uint16 PC, uint16 SP, uint8 A, uint16 BC, uint16 DE,
                      uint16 HL, bool carry, bool halfCarry, bool subtract,
                      bool zero, bool IME, uint8 intEnable, uint8 intFlags,
                      uint8 lcdc, uint8 stat, uint8 ly) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES,
           "CPU >>> PC: %04X | SP: %04X | A: %02X | BC: %04X | DE: %04X | HL: "
           "%04X | CHNZ: %d%d%d%d | IME: %d | IE: %02X | IF: %02X | LCDC: %02X "
           "| STAT: %02X | LY: %02X\n",
           PC, SP, A, BC, DE, HL, carry, halfCarry, subtract, zero, IME,
           intEnable, intFlags, lcdc, stat, ly);
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logCPUCycles(uint8 opcode, uint8 cbOpcode, uint8 cycles) {
  char logLine[LINE_BYTES];
  if (opcode == 0xCB) {
    snprintf(logLine, LINE_BYTES, "CPU >>> OPCODE %02X %02X TOOK %d CYCLES\n",
             opcode, cbOpcode, cycles);
  } else {
    snprintf(logLine, LINE_BYTES, "CPU >>> OPCODE %02X TOOK %d CYCLES\n",
             opcode, cycles);
  }
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logInterruptEnable(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "INT >>> ENABLED INTERRUPTS AT PC %04X\n", PC);
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logInterruptDisable(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "INT >>> DISABLED INTERRUPTS AT PC %04X\n", PC);
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logInterruptRequest(uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "INT >>> REQUESTED %s\n",
           intName.at(interrupt).c_str());
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logInterruptService(uint16 PC, uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "INT >>> SERVICED %s AT PC %04X\n",
           intName.at(interrupt).c_str(), PC);
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logInterruptReturn(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "INT >>> RETURNED TO PC %04X\n", PC);
  if (!Bootstrap::enabled && enable) log.write(logLine, strlen(logLine));
}

void Log::logStr(char *str) {
  if (!Bootstrap::enabled) log.write(str, strlen(str));
}
