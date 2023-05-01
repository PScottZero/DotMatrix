#include "log.h"

#include <chrono>

#include "interrupts.h"

fstream Log::cpuLog("/Users/paulscott/git/DotMatrix/debug/cpu_log.txt",
                    ios::out);
fstream Log::intLog("/Users/paulscott/git/DotMatrix/debug/interrupt_log.txt",
                    ios::out);
fstream Log::clockLog("/Users/paulscott/git/DotMatrix/debug/clock_log.txt",
                      ios::out);

mutex Log::cpuLogMutex;
mutex Log::intLogMutex;
mutex Log::clockLogMutex;

const map<uint8, string> Log::intName = {
    {V_BLANK_INT, "VBLANK"}, {LCDC_INT, "LCDC"},     {TIMER_INT, "TIMER"},
    {SERIAL_INT, "SERIAL"},  {JOYPAD_INT, "JOYPAD"},
};

const map<ClockType, string> Log::clockName = {
    {ClockType::CPU_CLOCK, "CPU"},
    {ClockType::PPU_CLOCK, "PPU"},
    {ClockType::TIMERS_CLOCK, "TIMERS"},
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
  cpuLogMutex.lock();
  Log::cpuLog.write(logLine, strlen(logLine));
  cpuLogMutex.unlock();
}

void Log::logInterruptEnable(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "ENABLED INTERRUPTS @ PC %04x\n", PC);
  intLogMutex.lock();
  Log::intLog.write(logLine, strlen(logLine));
  intLogMutex.unlock();
}

void Log::logInterruptDisable(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "DISABLED INTERRUPTS @ PC %04x\n", PC);
  intLogMutex.lock();
  Log::intLog.write(logLine, strlen(logLine));
  intLogMutex.unlock();
}

void Log::logInterruptRequest(uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "REQUESTED %s\n",
           intName.at(interrupt).c_str());
  intLogMutex.lock();
  Log::intLog.write(logLine, strlen(logLine));
  intLogMutex.unlock();
}

void Log::logInterruptService(uint16 PC, uint8 interrupt) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "SERVICED %s @ PC %04x\n",
           intName.at(interrupt).c_str(), PC);
  intLogMutex.lock();
  Log::intLog.write(logLine, strlen(logLine));
  intLogMutex.unlock();
}

void Log::logInterruptReturn(uint16 PC) {
  char logLine[LINE_BYTES];
  snprintf(logLine, LINE_BYTES, "RETURNED TO PC %04x\n", PC);
  intLogMutex.lock();
  Log::intLog.write(logLine, strlen(logLine));
  intLogMutex.unlock();
}

void Log::logClock(ClockType type, timepoint time) {
  char logLine[LINE_BYTES];
  auto now = chrono::system_clock::now().time_since_epoch().count();
  auto clock = chrono::time_point_cast<chrono::microseconds>(time)
                   .time_since_epoch()
                   .count();
  snprintf(logLine, LINE_BYTES, "%s %lld %lld\n", clockName.at(type).c_str(),
           now, clock);
  clockLogMutex.lock();
  Log::clockLog.write(logLine, strlen(logLine));
  clockLogMutex.unlock();
}
