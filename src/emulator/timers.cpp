#include "timers.h"

#include "clock.h"
#include "interrupts.h"

Timers::Timers(Memory &mem)
    : QThread(),
      div(mem.getByte(DIV)),
      tima(mem.getByte(TIMA)),
      tma(mem.getByte(TMA)),
      tac(mem.getByte(TAC)),
      timaFreqs{},
      timaCycles(),
      divCycles() {}

Timers::~Timers() {
  Clock::threadsRunning = false;
  wait();
}

void Timers::run() {
  while (Clock::threadsRunning) {
    if (!Clock::stop) {
      updateTimers();
      Clock::wait(TIMERS_CLOCK, 1);
    } else {
      Clock::reset();
    }
  }
}

void Timers::updateTimers() {
  // update TIMA timer
  if (timerEnabled()) {
    ++timaCycles;
    if (timaCycles >= timerFreq()) {
      // on TIMA overflow, set TIMA
      // to TMA and request a timer
      // interrupt
      if (++tima == 0) {
        tima = tma;
        Interrupts::request(TIMER_INT);
      }
      timaCycles -= timerFreq();
    }
  }

  // update DIV timer
  ++divCycles;
  if (divCycles >= DIV_CYCLES) {
    ++div;
    divCycles -= DIV_CYCLES;
  }
}

bool Timers::timerEnabled() { return tac & 0x04; }

uint8 Timers::timerFreq() { return timaFreqs[tac & 0b11]; }
