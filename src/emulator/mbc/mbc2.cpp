#include "mbc2.h"

#include "../memory.h"
#include "mbc.h"

uint8 MBC2::romBankNum = 1;

// **************************************************
// MBC2
//
// registers:
// 0000-3FFF: when addr bit 8 is
//   0: RAM enable <- 0A enable, otherwise disable
//   1: ROM bank number <- lower 4-bits of val
//
// ROM bank number bits: rrrr
//
// memory map:
// 0000-3FFF <- ROM bank 0000
// 4000-7FFF <- ROM bank rrrr
// A000-A1FF <- built-in RAM (512 half-bytes)
// A200-BFFF <- 15 echoes of A000-A1FF
// **************************************************
void MBC2::write(uint16 addr, uint8 val) {
  if (addr <= RAM_ENABLE_ROM_BANK_NUM) {
    if (addr & 0x10) {
      MBC::ramEnabled = val == 0x0A;
    } else {
      romBankNum = val & 0x0F;
      Memory::mapCartMem(Memory::romBank1, romBankNum);
    }
  }
}

void MBC2::reset() { romBankNum = 1; }
