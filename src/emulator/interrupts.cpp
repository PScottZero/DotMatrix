#include "interrupts.h"

uint8 *Interrupts::intEnable = nullptr;
uint8 *Interrupts::intFlags = nullptr;

void Interrupts::request(uint8 interrupt) { *intFlags |= interrupt; }

void Interrupts::reset(uint8 interrupt) { *intFlags &= ~interrupt; }

bool Interrupts::requestedAndEnabled(uint8 interrupt) {
  return (*intEnable & *intFlags & interrupt);
}

bool Interrupts::pending() { return (*intEnable) & (*intFlags) & 0x1f; }
