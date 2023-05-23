// **************************************************
// **************************************************
// **************************************************
// DIV & TIMA Timers
// **************************************************
// **************************************************
// **************************************************

#include "timers.h"

#include "cgb.h"

const uint16 Timers::internalCounterMasks[4]{TAC_00, TAC_01, TAC_10, TAC_11};

Timers::Timers() : cgb(nullptr), timaOverflow(false), internalCounter(0) {}

void Timers::step() {
  if (!cgb->stop) {
    // increment internal counter and
    // update DIV register
    uint16 oldInternalCounter = internalCounter;
    internalCounter += 4;
    cgb->mem.getByte(DIV) = (internalCounter & DIV_MASK) >> 8;

    // update TIMA timer if enabled
    if (timerEnabled()) {
      // TIMA overflow interrupt and modulo
      // delayed by one cycle
      if (timaOverflow) {
        cgb->mem.getByte(TIMA) = cgb->mem.getByte(TMA);
        cgb->cpu.requestInterrupt(TIMER_INT);
        timaOverflow = false;
      }

      // increment TIMA
      else {
        uint16 oldCounter =
            oldInternalCounter & internalCounterMasks[timerFreq()];
        uint16 counter = internalCounter & internalCounterMasks[timerFreq()];
        if (counter < oldCounter) {
          if (++cgb->mem.getByte(TIMA) == 0) {
            timaOverflow = true;
          }
        }
      }
    }
  }
}

// check if tima is enabled
bool Timers::timerEnabled() const { return cgb->mem.getByte(TAC) & BIT2_MASK; }

// return frequency for incrementing tima
uint8 Timers::timerFreq() const {
  return cgb->mem.getByte(TAC) & TWO_BITS_MASK;
}

// reset timers by setting internal counter,
// div, and tima to 0
void Timers::reset() {
  internalCounter = 0;
  cgb->mem.getByte(DIV) = 0;
}
