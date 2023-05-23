// **************************************************
// **************************************************
// **************************************************
// Audio Processing Unit (APU)
// **************************************************
// **************************************************
// **************************************************

#include "apu.h"

#include "types.h"

APU::APU() : cgb(nullptr) {}

// **************************************************
// **************************************************
// NR52 Sound On/Off Functions
// **************************************************
// **************************************************

bool APU::soundOn() { return cgb->mem.getByte(NR52) & BIT7_MASK; }

bool APU::channel4On() { return cgb->mem.getByte(NR52) & BIT3_MASK; }

bool APU::channel3On() { return cgb->mem.getByte(NR52) & BIT2_MASK; }

bool APU::channel2On() { return cgb->mem.getByte(NR52) & BIT1_MASK; }

bool APU::channel1On() { return cgb->mem.getByte(NR52) & BIT0_MASK; }

// **************************************************
// **************************************************
// NR51 Sound Panning Functions
// **************************************************
// **************************************************

bool APU::mixChannel4Left() { return cgb->mem.getByte(NR51) & BIT7_MASK; }

bool APU::mixChannel3Left() { return cgb->mem.getByte(NR51) & BIT6_MASK; }

bool APU::mixChannel2Left() { return cgb->mem.getByte(NR51) & BIT5_MASK; }

bool APU::mixChannel1Left() { return cgb->mem.getByte(NR51) & BIT4_MASK; }

bool APU::mixChannel4Right() { return cgb->mem.getByte(NR51) & BIT3_MASK; }

bool APU::mixChannel3Right() { return cgb->mem.getByte(NR51) & BIT2_MASK; }

bool APU::mixChannel2Right() { return cgb->mem.getByte(NR51) & BIT1_MASK; }

bool APU::mixChannel1Right() { return cgb->mem.getByte(NR51) & BIT0_MASK; }

// **************************************************
// **************************************************
// NR50 master volume & VIN panning
// **************************************************
// **************************************************

bool APU::mixVinLeft() { return cgb->mem.getByte(NR50) & BIT7_MASK; }

uint8 APU::leftVolume() {
  return (cgb->mem.getByte(NR50) >> 4) & THREE_BITS_MASK;
}

bool APU::mixVinRight() { return cgb->mem.getByte(NR50) & BIT3_MASK; }

uint8 APU::rightVolume() { return cgb->mem.getByte(NR50) & THREE_BITS_MASK; }

// **************************************************
// **************************************************
// Sound Channel 1 Functions
// **************************************************
// **************************************************

// **************************************************
// NR10 Sweep
// **************************************************
uint8 APU::channel1SweepPace() {
  return (cgb->mem.getByte(NR10) >> 4) & THREE_BITS_MASK;
}

bool APU::channel1IncreaseSweep() { return cgb->mem.getByte(NR10) & BIT3_MASK; }

uint8 APU::channel1SweepSlope() {
  return cgb->mem.getByte(NR10) & THREE_BITS_MASK;
}

// **************************************************
// NR11 Length Timer & Duty Cycle
// **************************************************
uint8 APU::channel1WaveDuty() {
  return (cgb->mem.getByte(NR11) >> 6) & TWO_BITS_MASK;
}

uint8 APU::channel1LengthTimer() {
  return cgb->mem.getByte(NR11) & SIX_BITS_MASK;
}

// **************************************************
// NR12 Volume & Envelope
// **************************************************
uint8 APU::channel1EnvelopeVolume() {
  return (cgb->mem.getByte(NR12) >> 4) & NIBBLE_MASK;
}

bool APU::channel1IncreaseEnvelope() {
  return cgb->mem.getByte(NR12) & BIT3_MASK;
}

uint8 APU::channel1EnvelopeSweepPace() {
  return cgb->mem.getByte(NR12) & THREE_BITS_MASK;
}

// **************************************************
// NR13 + NR14 Wavelength & Control
// **************************************************
uint16 APU::channel1Wavelength() {
  return ((cgb->mem.getByte(NR14) & THREE_BITS_MASK) << 8) |
         cgb->mem.getByte(NR13);
}

bool APU::triggerChannel1() { return cgb->mem.getByte(NR14) & BIT7_MASK; }

bool APU::channel1SoundLengthEnabled() {
  return cgb->mem.getByte(NR14) & BIT6_MASK;
}

// **************************************************
// **************************************************
// Sound Channel 2 Functions
// **************************************************
// **************************************************

// **************************************************
// NR21 Length Timer & Duty Cycle
// **************************************************
uint8 APU::channel2WaveDuty() {
  return (cgb->mem.getByte(NR21) >> 6) & TWO_BITS_MASK;
}

uint8 APU::channel2LengthTimer() {
  return cgb->mem.getByte(NR21) & SIX_BITS_MASK;
}

// **************************************************
// NR22 Volume & Envelope
// **************************************************
uint8 APU::channel2EnvelopeVolume() {
  return (cgb->mem.getByte(NR22) >> 4) & NIBBLE_MASK;
}

bool APU::channel2IncreaseEnvelope() {
  return cgb->mem.getByte(NR22) & BIT3_MASK;
}

uint8 APU::channel2EnvelopeSweepPace() {
  return cgb->mem.getByte(NR22) & THREE_BITS_MASK;
}

// **************************************************
// NR23 + NR24 Wavelength & Control
// **************************************************
uint16 APU::channel2Wavelength() {
  return ((cgb->mem.getByte(NR24) & THREE_BITS_MASK) << 8) |
         cgb->mem.getByte(NR23);
}

bool APU::triggerChannel2() { return cgb->mem.getByte(NR24) & BIT7_MASK; }

bool APU::channel2SoundLengthEnabled() {
  return cgb->mem.getByte(NR24) & BIT6_MASK;
}

// **************************************************
// **************************************************
// Sound Channel 3 Functions
// **************************************************
// **************************************************

// **************************************************
// NR30 DAC Enable
// **************************************************
bool APU::channel3DacEnable() { return cgb->mem.getByte(NR30) & BIT7_MASK; }

// **************************************************
// NR31 Length Timer
// **************************************************
uint8 APU::channel3LengthTimer() { return cgb->mem.getByte(NR31); }

// **************************************************
// NR32 Output Level
// **************************************************
uint8 APU::channel3OutputLevel() {
  return (cgb->mem.getByte(NR32) >> 5) & TWO_BITS_MASK;
}

// **************************************************
// NR33 + NR34 Wavelength & Control
// **************************************************
uint16 APU::channel3Wavelength() {
  return ((cgb->mem.getByte(NR34) & THREE_BITS_MASK) << 8) |
         cgb->mem.getByte(NR33);
}

bool APU::triggerChannel3() { return cgb->mem.getByte(NR34) & BIT7_MASK; }

bool APU::channel3SoundLengthEnabled() {
  return cgb->mem.getByte(NR34) & BIT6_MASK;
}

// **************************************************
// **************************************************
// Sound Channel 4 (Noise) Functions
// **************************************************
// **************************************************

// **************************************************
// NR41 Length Timer
// **************************************************
uint8 APU::channel4LengthTimer() {
  return cgb->mem.getByte(NR41) & SIX_BITS_MASK;
}

// **************************************************
// NR42 Volume & Envelope
// **************************************************
uint8 APU::channel4EnvelopeVolume() {
  return (cgb->mem.getByte(NR42) >> 4) & NIBBLE_MASK;
}

bool APU::channel4IncreaseEnvelope() {
  return cgb->mem.getByte(NR42) & BIT3_MASK;
}

uint8 APU::channel4EnvelopeSweepPace() {
  return cgb->mem.getByte(NR42) & THREE_BITS_MASK;
}

// **************************************************
// NR43 Frequency & Randomness
// **************************************************
uint8 APU::channel4ClockShift() {
  return (cgb->mem.getByte(NR43) >> 4) & NIBBLE_MASK;
}

bool APU::lfsrWidth() { return cgb->mem.getByte(NR43) & BIT3_MASK; }

uint8 APU::channel4ClockDivider() {
  return cgb->mem.getByte(NR43) & THREE_BITS_MASK;
}

// **************************************************
// NR44 Control
// **************************************************
bool APU::triggerChannel4() { return cgb->mem.getByte(NR44) & BIT7_MASK; }

bool APU::channel4SoundLengthEnabled() {
  return cgb->mem.getByte(NR44) & BIT6_MASK;
}
