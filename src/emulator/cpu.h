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

// register constants
#define NUM_REG_16 4
#define NUM_REG_8 8
#define MEM_HL 0b110

// jump condition constants
#define JUMP_NZ 0b00
#define JUMP_Z 0b01
#define JUMP_NC 0b10
#define JUMP_C 0b11

// interrupt handler addresses
#define VBLANK_INT_ADDR 0x40
#define LCDC_INT_ADDR 0x48
#define TIMER_INT_ADDR 0x50
#define SERIAL_INT_ADDR 0x58
#define JOYPAD_INT_ADDR 0x60

class CPU {
 private:
  static uint16 PC, SP, BC, DE, HL;        // 16-bit registers
  static uint8 A, &B, &C, &D, &E, &H, &L;  // 8-bit registers

  static bool zero, subtract, halfCarry, carry, IME, halt;  // flags

  static uint8 *regmap8[NUM_REG_8];
  static uint16 *regmap16[NUM_REG_16];

  static bool shouldSetIME, delaySetIME, triggerHaltBug;

  static uint16 serialTransferCycles;

  static void ppuTimerSerialStep(int cycles);
  static bool serialTransferComplete();

  // instruction decoding functions
  static void runInstr(uint8 opcode);
  static void runInstrCB(uint8 opcode);

  // transfer functions
  static void push(uint16 val);
  static uint16 pop();

  // arithmetic and logical functions
  static uint8 add(uint8 a, uint8 b, bool c = false);
  static void addHL(uint16 val);
  static uint16 addSP(int8 val);
  static uint8 sub(uint8 a, uint8 b, bool c = false);
  static void _and(uint8 val);
  static void _or(uint8 val);
  static void _xor(uint8 val);

  // rotate shift functions
  static uint8 rotateLeft(uint8 val, bool thruCarry = false);
  static uint8 rotateRight(uint8 val, bool thruCarry = false);
  static uint8 shiftLeft(uint8 val);
  static uint8 shiftRight(uint8 val, bool arithmetic = false);
  static uint8 swap(uint8 val);

  // bit functions
  static void bit(uint8 val, uint8 bitPos);
  static uint8 set(uint8 val, uint8 bitPos, bool bitVal);

  // jump functions
  static bool jumpCondMet(uint8 jumpCond);

  // general-purpose arithmetic functions
  static void decimalAdjAcc();

  // register AF setter + getter
  static uint16 getAF();
  static void setAF(uint16 val);

  // interrupt functions
  static void handleInterrupts();

 public:
  static bool serialTransferMode;

  static void step();
  static void reset();
};
