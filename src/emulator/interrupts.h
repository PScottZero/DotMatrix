#pragma once

#include "types.h"

// interrupt constants
#define JOYPAD_INT 0x10
#define SERIAL_INT 0x08
#define TIMER_INT 0x04
#define LCDC_INT 0x02
#define V_BLANK_INT 0x01

class Interrupts {
 public:
  static uint8 *intEnable, *intFlags;

  static void enable(uint8 interrupt);
  static void disable(uint8 interrupt);
  static void request(uint8 interrupt);
  static void reset(uint8 interrupt);
  static bool enabled(uint8 interrupt);
  static bool requestedAndEnabled(uint8 interrupt);
  static bool pending();
};
