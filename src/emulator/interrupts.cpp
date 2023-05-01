#include "interrupts.h"

#include "log.h"

uint8 *Interrupts::intEnable = nullptr;
uint8 *Interrupts::intFlags = nullptr;

void Interrupts::request(uint8 interrupt) {
  Log::logInterruptRequest(interrupt);
  *intFlags |= interrupt;
}

void Interrupts::reset(uint16 PC, uint8 interrupt) {
  Log::logInterruptService(PC, interrupt);
  *intFlags &= ~interrupt;
}

bool Interrupts::requestedAndEnabled(uint8 interrupt) {
  return (*intEnable & *intFlags & interrupt);
}

bool Interrupts::pending() { return (*intEnable) & (*intFlags) & 0x1f; }
