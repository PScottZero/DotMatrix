#include "timers.h"

#include "interrupts.h"

uint16 Timers::internalCounter = 0;

uint8 *Timers::div = nullptr;
uint8 *Timers::tima = nullptr;
uint8 *Timers::tma = nullptr;
uint8 *Timers::tac = nullptr;

const uint16 Timers::internalCounterMasks[4]{TAC_00, TAC_01, TAC_10, TAC_11};

void Timers::step(int cycles) {
  // increment internal counter and
  // update DIV register
  uint16 oldInternalCounter = internalCounter;
  internalCounter += 4 * cycles;
  *div = (internalCounter & DIV_MASK) >> 8;

  // update TIMA timer
  if (timerEnabled()) {
    uint16 oldCounter = oldInternalCounter & internalCounterMasks[timerFreq()];
    uint16 counter = internalCounter & internalCounterMasks[timerFreq()];
    if (counter < oldCounter) {
      if (++*tima == 0) {
        *tima = *tma;
        Interrupts::request(TIMER_INT);
      }
    }
  }
}

bool Timers::timerEnabled() { return *tac & 0x04; }

uint8 Timers::timerFreq() { return *tac & 0b11; }
