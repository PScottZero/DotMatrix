// **************************************************
// **************************************************
// **************************************************
// BOOTSTRAP (RUNS WHEN GAME BOY BOOTS)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include "types.h"

#define DMG_BOOTSTRAP_BYTES 0x100
#define CGB_BOOTSTRAP_BYTES 0x900
#define CGB_BOOTSTRAP_ADDR 0x200

class Bootstrap {
 private:
  static uint8 dmgBootstrap[DMG_BOOTSTRAP_BYTES];
  static uint8 cgbBootstrap[CGB_BOOTSTRAP_BYTES];

 public:
  static bool enabled, skip;

  static uint8 &at(uint16 addr);
  static bool enabledAndShouldSkip();
  static void reset();
};
