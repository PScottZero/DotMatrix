// **************************************************
// **************************************************
// **************************************************
// AUDIO PROCESSING UNIT (APU)
// **************************************************
// **************************************************
// **************************************************

#pragma once

// #include "OpenAL/OpenAL.h"
#include "types.h"

class APU {
 private:
  //  static ALCdevice *device;
  //  static ALCcontext *context;
  static uint8 &nr10, &nr11, &nr12, &nr13, &nr14, &nr21, &nr22, &nr23, &nr24,
      &nr30, &nr31, &nr32, &nr33, &nr34, &nr41, &nr42, &nr43, &nr44, &nr50,
      &nr51, &nr52;

  // sound enabled functions
  static bool soundEnabled();
  static bool channel1Enabled();
  static bool channel2Enabled();
  static bool channel3Enabled();
  static bool channel4Enabled();

 public:
  void step();
};
