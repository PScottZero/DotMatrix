#include "interrupts.h"

uint8 *Interrupts::intEnable = nullptr;
uint8 *Interrupts::intFlags = nullptr;

void Interrupts::enable(uint8 interrupt) { *intEnable |= interrupt; }

void Interrupts::disable(uint8 interrupt) { *intEnable &= ~interrupt; }

void Interrupts::request(uint8 interrupt) { *intFlags |= interrupt; }

void Interrupts::reset(uint8 interrupt) { *intFlags &= ~interrupt; }

bool Interrupts::enabled(uint8 interrupt) { return *intEnable & interrupt; }

bool Interrupts::requestedAndEnabled(uint8 interrupt) {
  return enabled(interrupt) && (*intFlags & interrupt);
}

bool Interrupts::pending() {
  return (*intEnable) & (*intFlags) & 0x1f;
}
