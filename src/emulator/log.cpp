#include "log.h"

std::fstream Log::cpuLog("/Users/paulscott/git/DotMatrix/debug/cpu_log.txt",
                         std::ios::out);

std::fstream Log::intLog(
    "/Users/paulscott/git/DotMatrix/debug/interrupt_log.txt", std::ios::out);

const std::map<uint8, std::string> Log::intName = {
    {V_BLANK_INT, "VBLANK"}, {LCDC_INT, "LCDC"},     {TIMER_INT, "TIMER"},
    {SERIAL_INT, "SERIAL"},  {JOYPAD_INT, "JOYPAD"},
};

void Log::logCPUState(uint16 PC, uint16 SP, uint8 A, uint16 BC, uint16 DE,
                      uint16 HL, uint8 lcdc, uint8 stat, uint8 ly, uint8 scx,
                      uint8 scy, bool IME, uint8 intEnable, uint8 intFlags,
                      bool carry, bool halfCarry, bool subtract, bool zero) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES,
           "PC: %04x | SP: %04x | A: %02x | BC: %04x | DE: %04x | HL: %02x | "
           "LCDC: %02x | STAT: %02x | LY: %02x | SCX: %02x | SCY: %02x | "
           "IME: %d | IE: %02x | IF: %02x | CHNZ: %d%d%d%d\n",
           PC, SP, A, BC, DE, HL, lcdc, stat, ly, scx, scy, IME, intEnable,
           intFlags, carry, halfCarry, subtract, zero);
  Log::cpuLog.write(logLine, strlen(logLine));
}

void Log::logInterruptRequest(uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "REQUESTED %s\n",
           intName.at(interrupt).c_str());
  Log::intLog.write(logLine, strlen(logLine));
}

void Log::logInterruptService(uint16 PC, uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "SERVICED %s @ PC %04x\n",
           intName.at(interrupt).c_str(), PC);
  Log::intLog.write(logLine, strlen(logLine));
}

void Log::logInterruptReturn(uint16 PC, uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "RETURNED %s @ PC %04x\n",
           intName.at(interrupt).c_str(), PC);
  Log::intLog.write(logLine, strlen(logLine));
}
