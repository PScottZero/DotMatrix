// **************************************************
// **************************************************
// **************************************************
// Audio Processing Unit (APU)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <array>

#include "types.h"

#define SWEEP_BASE_TICKS 128
#define CHANNEL_COUNT 4
#define CYCLES_PER_TICK 8192

using namespace std;

class CGB;

class APU {
 private:
  array<float, CHANNEL_COUNT> sweepCycles;
  array<float, CHANNEL_COUNT> lengthCycles;

  uint16 channel1CurrSweepPace;

 public:
  APU();
  CGB *cgb;

  void step();

  void channel1Step();
  void channel2Step();
  void channel3Step();
  void channel4Step();

  // NR52 sound on/off functions
  bool soundOn();
  bool channel4On();
  bool channel3On();
  bool channel2On();
  bool channel1On();
  void disableChannel4();
  void disableChannel3();
  void disableChannel2();
  void disableChannel1();

  // NR51 sound panning functions
  bool mixChannel4Left();
  bool mixChannel3Left();
  bool mixChannel2Left();
  bool mixChannel1Left();
  bool mixChannel4Right();
  bool mixChannel3Right();
  bool mixChannel2Right();
  bool mixChannel1Right();

  // NR50 master volume & VIN panning
  bool mixVinLeft();
  uint8 leftVolume();
  bool mixVinRight();
  uint8 rightVolume();

  // NR10 channel 1 sweep
  uint8 channel1SweepPace();
  bool channel1IncreaseSweep();
  uint8 channel1SweepSlope();

  // NR11 channel 1 length timer & duty cycle
  uint8 channel1WaveDuty();
  uint8 channel1LengthTimer();

  // NR12 channel 1 volume & envelope
  uint8 channel1EnvelopeVolume();
  bool channel1IncreaseEnvelope();
  uint8 channel1EnvelopeSweepPace();

  // NR13 + NR14 channel 1 wavelength & control
  uint16 channel1Wavelength();
  void updateChannel1Wavelength();
  bool triggerChannel1();
  bool channel1SoundLengthEnabled();

  // NR21 channel 2 length timer & duty cycle
  uint8 channel2WaveDuty();
  uint8 channel2LengthTimer();

  // NR22 channel 2 volume & envelope
  uint8 channel2EnvelopeVolume();
  bool channel2IncreaseEnvelope();
  uint8 channel2EnvelopeSweepPace();

  // NR23 + NR14 channel 2 wavelength & control
  uint16 channel2Wavelength();
  void setChannel2Wavelength(uint16 wavelength);
  bool triggerChannel2();
  bool channel2SoundLengthEnabled();

  // NR30 channel 3 dac enable
  bool channel3DacEnable();

  // NR31 channel 3 length timer
  uint8 channel3LengthTimer();

  // NR32 channel 3 output level
  uint8 channel3OutputLevel();

  // NR33 + NR34 channel 3 wavelength & control
  uint16 channel3Wavelength();
  void setChannel3Wavelength(uint16 wavelength);
  bool triggerChannel3();
  bool channel3SoundLengthEnabled();

  // NR41 channel 4 length timer
  uint8 channel4LengthTimer();

  // NR42 channel 4 volume & envelope
  uint8 channel4EnvelopeVolume();
  bool channel4IncreaseEnvelope();
  uint8 channel4EnvelopeSweepPace();

  // NR43 channel 4 frequency & randomness
  uint8 channel4ClockShift();
  bool lfsrWidth();
  uint8 channel4ClockDivider();

  // NR44 channel 4 control
  bool triggerChannel4();
  bool channel4SoundLengthEnabled();
};
