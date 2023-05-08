#include "interrupts.h"

#include "log.h"
#include "memory.h"

uint8 &Interrupts::intEnable = Memory::getByte(IE);
uint8 &Interrupts::intFlags = Memory::getByte(IF);

void Interrupts::request(uint8 interrupt) {
  Log::logInterruptRequest(interrupt);
  intFlags |= interrupt;
}

void Interrupts::reset(uint16 PC, uint8 interrupt) {
  Log::logInterruptService(PC, interrupt);
  intFlags &= ~interrupt;
}

bool Interrupts::requestedAndEnabled(uint8 interrupt) {
  return (intEnable & intFlags & interrupt);
}

bool Interrupts::pending() { return intEnable & intFlags & 0x1f; }
