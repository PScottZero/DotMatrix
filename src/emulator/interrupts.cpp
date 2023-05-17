// **************************************************
// **************************************************
// **************************************************
// INTERRUPT CONTROL (IE + IF REGISTER CONTROL)
// **************************************************
// **************************************************
// **************************************************

#include "interrupts.h"

#include "log.h"

Interrupts::Interrupts() : intEnable(), intFlags() {}

void Interrupts::request(uint8 interrupt) {
  Log::logInterruptRequest(interrupt);
  *intFlags |= interrupt;
}

void Interrupts::reset(uint16 PC, uint8 interrupt) {
  Log::logInterruptService(PC, interrupt);
  *intFlags &= ~interrupt;
}

bool Interrupts::requestedAndEnabled(uint8 interrupt) const {
  return (*intEnable & *intFlags & interrupt);
}

bool Interrupts::pending() const {
  return *intEnable & *intFlags & FIVE_BITS_MASK;
}
