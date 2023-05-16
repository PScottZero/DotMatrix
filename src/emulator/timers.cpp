// **************************************************
// **************************************************
// **************************************************
// TIMERS CONTROLLER (DIV, TIMA)
// **************************************************
// **************************************************
// **************************************************

#include "timers.h"

#include "cgb.h"
#include "interrupts.h"

const uint16 Timers::internalCounterMasks[4]{TAC_00, TAC_01, TAC_10, TAC_11};

Timers::Timers()
    : cgb(nullptr),
      div(nullptr),
      tima(nullptr),
      tma(nullptr),
      tac(nullptr),
      internalCounter(0) {}

void Timers::step() {
  if (!cgb->stop) {
    // increment internal counter and
    // update DIV register
    uint16 oldInternalCounter = internalCounter;
    internalCounter += 4;
    *div = (internalCounter & DIV_MASK) >> 8;

    // update TIMA timer if enabled
    if (timerEnabled()) {
      uint16 oldCounter =
          oldInternalCounter & internalCounterMasks[timerFreq()];
      uint16 counter = internalCounter & internalCounterMasks[timerFreq()];
      if (counter < oldCounter) {
        if (++*tima == 0) {
          *tima = *tma;
          cgb->interrupts.request(TIMER_INT);
        }
      }
    }
  }
}

// check if tima is enabled
bool Timers::timerEnabled() { return *tac & BIT2_MASK; }

// return frequency for incrementing tima
uint8 Timers::timerFreq() { return *tac & TWO_BITS_MASK; }

// reset timers by setting internal counter,
// div, and tima to 0
void Timers::reset() {
  internalCounter = 0;
  *div = 0;
}
