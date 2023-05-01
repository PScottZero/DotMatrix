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

#include "memory.h"

// register constants
#define NUM_REG_16 4
#define NUM_REG_8 8
#define MEM_HL 0b110

// jump condition constants
#define JUMP_NZ 0b00
#define JUMP_Z 0b01
#define JUMP_NC 0b10
#define JUMP_C 0b11

class CPU : public QThread {
  Q_OBJECT

 private:
  uint16 PC, SP, BC, DE, HL;        // 16-bit registers
  uint8 A, &B, &C, &D, &E, &H, &L;  // 8-bit registers

  bool zero, subtract, halfCarry, carry, IME, halt;  // flags

  uint8 *regmap8[NUM_REG_8];
  uint16 *regmap16[NUM_REG_16];

  Memory &mem;

  bool shouldSetIME;

  // instruction decoding functions
  void runInstr(uint8 instr, uint8 &cycles);
  void runInstrCB(uint8 instr, uint8 &cycles);

  // transfer functions
  void push(uint16 val, uint8 &cycles);
  uint16 pop(uint8 &cycles);

  // arithmetic and logical functions
  uint8 add(uint8 a, uint8 b, bool c = false);
  uint16 add(uint16 a, uint16 b);
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
  bool jumpCondMet(uint8 jumpCond);

  // general-purpose arithmetic functions
  void decimalAdjAcc();

  // register AF setter + getter
  uint16 getAF();
  void setAF(uint16 val);

  // interrupt functions
  void handleInterrupts(uint8 &cycles);

 public:
  CPU(Memory &mem);

  void run() override;
  void reset();

  void loadState();
  void saveState();
};
