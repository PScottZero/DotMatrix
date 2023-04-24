#pragma once

#include <stdio.h>

#include <QThread>
#include <chrono>
#include <fstream>
#include <thread>

#include "memory.h"

// clock speed constants
#define DMG_CLOCK_SPEED 0x100000  // 1MHz
#define CGB_CLOCK_SPEED 0x200000  // 2MHz
#ifndef NS_PER_SEC
#define NS_PER_SEC 1000000000
#endif

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
#define JOYPAD_INT 0x10
#define SERIAL_INT 0x08
#define TIMER_INT 0x04
#define LCDC_INT 0x02
#define V_BLANK_INT 0x01

class CPU : public QThread {
  Q_OBJECT

 private:
  uint16 PC, SP, BC, DE, HL;        // 16-bit registers
  uint8 A, &B, &C, &D, &E, &H, &L;  // 8-bit registers

  bool zero, subtract, halfCarry, carry, IME, halt, &stop;  // flags

  uint8 *regmap8[NUM_REG_8];
  uint16 *regmap16[NUM_REG_16];

  Memory &mem;

  uint8 &intEnable, &intFlags;

  float &speedMult;
  bool &threadRunning;
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
  void enableInterrupt(uint8 interrupt);
  void disableInterrupt(uint8 interrupt);
  void resetInterrupt(uint8 interrupt);
  bool interruptEnabled(uint8 interrupt);
  bool interruptRequested(uint8 interrupt);

  void log(std::fstream &fs);

 public:
  CPU(Memory &mem, float &speedMult, bool &stop, bool &threadRunning);
  ~CPU();

  void run() override;
  void setPC(uint16 addr);

  // request interrupt function
  void requestInterrupt(uint8 interrupt);
};
