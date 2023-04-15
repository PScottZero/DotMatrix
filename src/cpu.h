#pragma once

#include "memory.h"

class CPU : public QThread {
  Q_OBJECT

 private:
  uint16 PC, SP, BC, DE, HL;        // 16-bit registers
  uint8 A, &B, &C, &D, &E, &H, &L;  // 8-bit registers

  bool zero, subtract, halfCarry, carry;  // flags

  bool IME;
  bool halt;
  bool stop;

  uint8 *regmap8[NUM_REG_8];
  uint16 *regmap16[NUM_REG_16];

  Memory &mem;

  uint8 &intEnable;
  uint8 &intFlag;

  float &speedMult;
  bool running;

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
  void compare(uint8 val);
  uint8 increment(uint8 val);
  uint8 decrement(uint8 val);

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
  void handleInterrupts();
  void enableInterrupt(uint8 interrupt);
  void disableInterrupt(uint8 interrupt);
  bool interruptEnabled(uint8 interrupt);
  bool interruptTriggered(uint8 interrupt);

 public:
  CPU(Memory &mem, float &speedMult);
  ~CPU();

  void run() override;
};
