#include "mbc1.h"

#include "../memory.h"
#include "mbc.h"

bool MBC1::bankMode = false;
uint8 MBC1::romBankNum = 1;
uint8 MBC1::ramBankNum = 0;

// **************************************************
// MBC1
//
// registers:
// 0000-1FFF: RAM enable <- 0A enable, 00 disable
// 2000-3FFF: ROM bank number <- lower 5-bits of val
// 4000-5FFF: RAM bank numer <- lower 2-bits of val
// 6000-7FFF: bank mode <- lowest bit of val
//
// ROM bank number bits: rrrrr
// RAM bank number bits: ee
//
// bank mode 0 memory map:
// 0000-3FFF <- ROM bank 0000000
// 4000-7FFF <- ROM bank eerrrrr
// A000-BFFF <- RAM bank 00
//
// bank mode 1 memory map:
// 0000-3FFF <- ROM bank ee00000
// 4000-7FFF <- ROM bank eerrrrr
// A000-BFFF <- RAM bank ee
// **************************************************
void MBC1::write(uint16 addr, uint8 val) {
  // RAM enable register
  if (addr <= RAM_ENABLE) {
    MBC::ramEnabled = (val & 0x0F) == 0x0A && MBC::hasRAM();
  }

  // rom bank number register
  else if (addr <= ROM_BANK_NUM) {
    romBankNum = max(val & 0x1F, 1);
    Memory::mapCartMem(Memory::romBank1, romBank1BankNum());
  }

  // ram bank number register
  else if (addr <= RAM_BANK_NUM) {
    ramBankNum = val & 0b11;
    Memory::mapEXRAM(exramBankNum());
    Memory::mapCartMem(Memory::romBank0, romBank1BankNum());
    Memory::mapCartMem(Memory::romBank1, romBank1BankNum());
  }

  // bank mode select register
  else if (addr <= BANK_MODE) {
    bankMode = val & 0b1;
    Memory::mapEXRAM(exramBankNum());
    Memory::mapCartMem(Memory::romBank0, romBank0BankNum());
  }
}

uint8 MBC1::romBank0BankNum() { return bankMode ? ramBankNum << 5 : 0; }

uint8 MBC1::romBank1BankNum() {
  uint8 mask = pow(2, MBC::cartSize + 1) - 1;
  return (ramBankNum << 5 | romBankNum) & mask;
}

uint8 MBC1::exramBankNum() { return bankMode ? ramBankNum : 0; }

void MBC1::reset() {
  bankMode = false;
  romBankNum = 1;
  ramBankNum = 0;
}
