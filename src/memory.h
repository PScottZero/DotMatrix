#pragma once

#include "utils.h"

using namespace std;

class Memory {
 private:
  uint8 *mem;
  uint8 *cart;
  uint8 **romBank0;
  uint8 **romBank1;

  void mapCartMem(uint8 **romBank, uint16 startAddr);

  // access functions
  bool canAccessVRAM() const;
  bool canAccessOAM() const;

 public:
  Memory();
  ~Memory();

  void init();

  void loadROM(QString dir);

  // memory read + write functions
  uint8 read(uint16 addr, uint8 &cycles) const;
  uint16 read16(uint16 addr, uint8 &cycles) const;
  void write(uint16 addr, uint8 val, uint8 &cycles);
  void write(uint16 addr, uint16 val, uint8 &cycles);
  uint8 imm8(uint16 &PC, uint8 &cycles) const;
  uint16 imm16(uint16 &PC, uint8 &cycles) const;

  // direct memory access functions
  uint8 getByte(uint16 addr) const;
  uint16 getTwoBytes(uint16 addr) const;
  uint8 *getPtr(uint16 addr);
  uint8 &getRef(uint16 addr);
};
