// **************************************************
// **************************************************
// **************************************************
// AUDIO PROCESSING UNIT (APU)
// **************************************************
// **************************************************
// **************************************************

#include "apu.h"

#include "memory.h"

// initialize audio device and context
// ALCdevice *APU::device = alcOpenDevice(nullptr);
// ALCcontext *APU::context = alcCreateContext(device, nullptr);

// set sound register references
uint8 &APU::nr10 = Memory::getByte(NR10);
uint8 &APU::nr11 = Memory::getByte(NR11);
uint8 &APU::nr12 = Memory::getByte(NR12);
uint8 &APU::nr13 = Memory::getByte(NR13);
uint8 &APU::nr14 = Memory::getByte(NR14);
uint8 &APU::nr21 = Memory::getByte(NR21);
uint8 &APU::nr22 = Memory::getByte(NR22);
uint8 &APU::nr23 = Memory::getByte(NR23);
uint8 &APU::nr24 = Memory::getByte(NR24);
uint8 &APU::nr30 = Memory::getByte(NR30);
uint8 &APU::nr31 = Memory::getByte(NR31);
uint8 &APU::nr32 = Memory::getByte(NR32);
uint8 &APU::nr33 = Memory::getByte(NR33);
uint8 &APU::nr34 = Memory::getByte(NR34);
uint8 &APU::nr41 = Memory::getByte(NR41);
uint8 &APU::nr42 = Memory::getByte(NR42);
uint8 &APU::nr43 = Memory::getByte(NR43);
uint8 &APU::nr44 = Memory::getByte(NR44);
uint8 &APU::nr50 = Memory::getByte(NR50);
uint8 &APU::nr51 = Memory::getByte(NR51);
uint8 &APU::nr52 = Memory::getByte(NR52);

void APU::step() {}

// **************************************************
// **************************************************
// Sound Enabled Functions
// **************************************************
// **************************************************
bool APU::soundEnabled() { return nr52 & BIT7_MASK; }

bool APU::channel1Enabled() { return nr52 & BIT0_MASK; }

bool APU::channel2Enabled() { return nr52 & BIT1_MASK; }

bool APU::channel3Enabled() { return nr52 & BIT2_MASK; }

bool APU::channel4Enabled() { return nr52 & BIT3_MASK; }
