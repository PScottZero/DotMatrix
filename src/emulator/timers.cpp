#include "timers.h"

Timers::Timers(CPU &cpu, Memory &mem, float &speedMult, bool &stop,
               bool &threadRunning)
    : QThread(),
      cpu(cpu),
      div(mem.getByte(DIV)),
      tima(mem.getByte(TIMA)),
      tma(mem.getByte(TMA)),
      tac(mem.getByte(TAC)),
      timaFreqs{},
      timaCycles(),
      divCycles(),
      speedMult(speedMult),
      stop(stop),
      threadRunning(threadRunning) {}

Timers::~Timers() {
  threadRunning = false;
  wait();
}

void Timers::run() {
  while (threadRunning) {
    if (!stop) {
      auto start = std::chrono::system_clock::now();
      int cycleTimeNs = NS_PER_SEC / (TIMER_BASE_FREQ * speedMult);
      auto end = start + std::chrono::nanoseconds(cycleTimeNs);

      updateTimers();

      std::this_thread::sleep_until(end);
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
        cpu.requestInterrupt(TIMER_INT);
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
