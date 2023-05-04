#pragma once

#include "types.h"

// interrupt constants
#define V_BLANK_INT 0x01
#define LCDC_INT 0x02
#define TIMER_INT 0x04
#define SERIAL_INT 0x08
#define JOYPAD_INT 0x10

class Interrupts {
 public:
  static uint8 &intEnable, &intFlags;

  static void request(uint8 interrupt);
  static void reset(uint16 PC, uint8 interrupt);
  static bool requestedAndEnabled(uint8 interrupt);
  static bool pending();
};
