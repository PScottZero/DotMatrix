#pragma once

#include "types.h"

#define BOOTSTRAP_BYTES 0x100

class Bootstrap {
 private:
  static uint8 bootstrap[BOOTSTRAP_BYTES];

 public:
  static float speedMult;
  static bool enabled;
  static uint8 *at(uint16 addr);
};
