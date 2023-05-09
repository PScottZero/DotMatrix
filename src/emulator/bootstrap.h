// **************************************************
// **************************************************
// **************************************************
// BOOTSTRAP (RUNS WHEN GAME BOY BOOTS)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include "types.h"

#define BOOTSTRAP_BYTES 0x100

class Bootstrap {
 private:
  static uint8 bootstrap[BOOTSTRAP_BYTES];

 public:
  static bool enabled, skip;

  static uint8 &at(uint16 addr);
  static bool enabledAndShouldSkip();
  static void reset();
};
