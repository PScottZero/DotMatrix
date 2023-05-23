// **************************************************
// **************************************************
// **************************************************
// Sharp LR35902 Central Processing Unit (CPU)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <stdio.h>

#include <QThread>
#include <chrono>
#include <fstream>
#include <thread>

#include "types.h"

#define CPU_CLOCK_SPEED 0x100000
#define SERIAL_TRANSFER_SPEED 0x2000
#define SERIAL_TRANSFER_CYCLES (CPU_CLOCK_SPEED / SERIAL_TRANSFER_SPEED) * 8

// register constants
#define NUM_REG_16 4
#define NUM_REG_8 8
#define MEM_HL 0b110

// jump condition constants
#define JUMP_NZ 0b00
#define JUMP_Z 0b01
#define JUMP_NC 0b10
#define JUMP_C 0b11

// interrupt constants
#define VBLANK_INT_ADDR 0x40
#define LCDC_INT_ADDR 0x48
#define TIMER_INT_ADDR 0x50
#define SERIAL_INT_ADDR 0x58
#define JOYPAD_INT_ADDR 0x60
#define VBLANK_INT 0x01
#define LCDC_INT 0x02
#define TIMER_INT 0x04
#define SERIAL_INT 0x08
#define JOYPAD_INT 0x10

class CGB;

class CPU {
 private:
  uint16 PC, SP, BC, DE, HL;        // 16-bit registers
  uint8 A, &B, &C, &D, &E, &H, &L;  // 8-bit registers

  bool zero, subtract, halfCarry, carry, IME, halt;  // flags

  uint8 *regmap8[NUM_REG_8];
  uint16 *regmap16[NUM_REG_16];

  bool shouldSetIME, delaySetIME, triggerHaltBug;

  uint16 serialTransferCycles;

  bool serialTransferComplete();

  // instruction decoding functions
  void runInstr(uint8 opcode);
  void runInstrCB(uint8 opcode);

  // transfer functions
  void push(uint16 val);
  uint16 pop();

  // arithmetic and logical functions
  uint8 add(uint8 a, uint8 b, bool c = false);
  void addHL(uint16 val);
  uint16 addSP(int8 val);
  uint8 sub(uint8 a, uint8 b, bool c = false);
  void _and(uint8 val);
  void _or(uint8 val);
  void _xor(uint8 val);

  // rotate shift functions
  uint8 rotateLeft(uint8 val, bool thruCarry = false);
  uint8 rotateRight(uint8 val, bool thruCarry = false);
  uint8 shiftLeft(uint8 val);
  uint8 shiftRight(uint8 val, bool arithmetic = false);
  uint8 swap(uint8 val);

  // bit functions
  void bit(uint8 val, uint8 bitPos);
  uint8 set(uint8 val, uint8 bitPos, bool bitVal);

  // jump functions
  bool jumpCondMet(uint8 jumpCond) const;

  // general-purpose arithmetic functions
  void decimalAdjAcc();

  // register AF setter + getter
  uint16 getAF() const;
  void setAF(uint16 val);

  // private interrupt functions
  void handleInterrupts();
  void resetInterrupt(uint16 PC, uint8 interrupt);
  bool interruptRequestedAndEnabled(uint8 interrupt) const;
  bool interruptsPending() const;

 public:
  bool serialTransferMode;
  CGB *cgb;

  CPU();

  void step();
  void ppuTimerSerialStep(int cycles);
  void reset();

  // public interrupt function
  void requestInterrupt(uint8 interrupt);
};
