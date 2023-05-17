// **************************************************
// **************************************************
// **************************************************
// MEMORY BANK CONTROLLER (MBC)
// **************************************************
// **************************************************
// **************************************************

#include "mbc.h"

#include <math.h>

#include "memory.h"

MBC::MBC()
    : mem(nullptr),
      bankType(ROM_ONLY),
      romSize(ROM_SIZE_32KB),
      ramSize(NO_RAM),
      ramEnabled(false),
      romBankNum(1),
      ramBankNum(0),
      romBankBit9(false),
      bankMode(false),
      halfRAMMode(false) {}

// **************************************************
// **************************************************
// MBC Functions
// **************************************************
// **************************************************

// write to MBC registers
void MBC::write(uint16 addr, uint8 val) {
  switch (bankType) {
    case MBC1_:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
      mbc1(addr, val);
      break;
    case MBC2_:
    case MBC2_BATTERY:
      mbc2(addr, val);
      break;
    case MBC5_:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
      mbc5(addr, val);
      break;
  }
}

// **************************************************
// MBC1
//
// registers:
// 0000-1FFF: ram enable <- val & F = A enable
// 2000-3FFF: rom bank number <- val & 1F
// 4000-5FFF: ram bank numer <- val & 3
// 6000-7FFF: bank mode <- val & 1
//
// rom bank number bits: rrrrr
// ram bank number bits: qq
//
// bank mode 0 memory map:
// 0000-3FFF <- rom bank 0000000
// 4000-7FFF <- rom bank qqrrrrr
// A000-BFFF <- ram bank 00
//
// bank mode 1 memory map:
// 0000-3FFF <- rom bank qq00000
// 4000-7FFF <- rom bank qqrrrrr
// A000-BFFF <- ram bank qq
// **************************************************
void MBC::mbc1(uint16 addr, uint8 val) {
  // RAM enable register
  if (addr <= MBC1_RAM_ENABLE) {
    ramEnabled = (val & NIBBLE_MASK) == 0x0A && MBC::hasRam();
  }

  // rom bank number register
  else if (addr <= MBC1_ROM_BANK_NUM) {
    romBankNum = max(val & FIVE_BITS_MASK, 1);
    mem->setRomBank(&mem->romBank1, mbc1RomBank1BankNum());
  }

  // ram bank number register
  else if (addr <= MBC1_RAM_BANK_NUM) {
    ramBankNum = val & TWO_BITS_MASK;
    mem->setExramBank(mbc1ExramBankNum());
    mem->setRomBank(&mem->romBank0, mbc1RomBank0BankNum());
    mem->setRomBank(&mem->romBank1, mbc1RomBank1BankNum());
  }

  // bank mode select register
  else if (addr <= MBC1_BANK_MODE) {
    bankMode = val & BIT0_MASK;
    mem->setExramBank(mbc1ExramBankNum());
    mem->setRomBank(&mem->romBank0, mbc1RomBank0BankNum());
  }
}

// **************************************************
// MBC2
//
// registers:
// 0000-3FFF: when addr bit 8 is
//   0: ram enable <- val & F = A enable
//   1: rom bank number <- val & F
//
// rom bank number bits: rrrr
//
// memory map:
// 0000-3FFF <- rom bank 0000
// 4000-7FFF <- rom bank rrrr
// A000-A1FF <- built-in RAM (512 half-bytes)
// A200-BFFF <- 15 echoes of A000-A1FF
// **************************************************
void MBC::mbc2(uint16 addr, uint8 val) {
  if (addr <= MBC2_RAM_ROM) {
    if (addr & BIT8_MASK) {
      romBankNum = max(val & NIBBLE_MASK, 1) & romSizeMask();
      mem->setRomBank(&mem->romBank1, romBankNum);
    } else {
      ramEnabled = (val & NIBBLE_MASK) == 0x0A;
    }
  }
}

// **************************************************
// MBC5
//
// registers:
// 0000-1FFF: ram enable <- val & F = A enable
// 2000-2FFF: rom bank number lower byte <- val
// 3000-3FFF: rom bank number bit 9 <- val & 1
// 4000-5FFF: ram bank number <- val & F
//
// rom bank number lower byte bits: rrrrrrrr
// rom bank number bit 9: q
// ram bank number bits: pppp
//
// memory map:
// 0000-3FFF <- rom bank 000000000
// 4000-7FFF <- rom bank qrrrrrrrr
// A000-BFFF <- ram bank pppp
// **************************************************
void MBC::mbc5(uint16 addr, uint8 val) {
  // ram enable
  if (addr <= MBC5_RAM_ENABLE) {
    ramEnabled = (val & NIBBLE_MASK) == 0x0A;
  }

  // rom bank number lower byte
  else if (addr <= MBC5_ROM_BANK_BYTE_LO) {
    romBankNum = val;
    mem->setRomBank(&mem->romBank1, mbc5RomBankNum());
  }

  // rom bank number bit 9
  else if (addr <= MBC5_ROM_BANK_BIT_9) {
    romBankBit9 = val & BIT0_MASK;
    mem->setRomBank(&mem->romBank1, mbc5RomBankNum());
  }

  // ram bank number
  else if (addr <= MBC5_RAM_BANK_NUM) {
    if (val <= 0x0F) {
      ramBankNum = val;
      mem->setExramBank(mbc5RamBankNum());
    }
  }
}

// **************************************************
// **************************************************
// MBC Helper Functions
// **************************************************
// **************************************************

uint8 MBC::mbc1RomBank0BankNum() const {
  return bankMode ? (ramBankNum << 5) & romSizeMask() : 0;
}

uint8 MBC::mbc1RomBank1BankNum() const {
  return (ramBankNum << 5 | romBankNum) & romSizeMask();
}

uint8 MBC::mbc1ExramBankNum() const {
  return bankMode ? ramBankNum & ramSizeMask() : 0;
}

uint16 MBC::mbc5RomBankNum() const {
  return ((romBankBit9 << 9) | romBankNum) & romSizeMask();
}

uint8 MBC::mbc5RamBankNum() const { return ramBankNum & ramSizeMask(); }

// **************************************************
// **************************************************
// MBC Config Functions
// **************************************************
// **************************************************

uint8 MBC::romSizeMask() const { return pow(2, romSize + 1) - 1; }

uint8 MBC::ramSizeMask() const { return ramSize == RAM_SIZE_8KB ? 0b00 : 0b11; }

string MBC::bankTypeStr() const {
  switch (bankType) {
    case ROM_ONLY:
      return "ROM Only";
    case MBC1_:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
      return "MBC1";
    case MBC2_:
    case MBC2_BATTERY:
      return "MBC2";
    case MMM01_:
    case MMM01_RAM:
    case MMM01_RAM_BATTERY:
      return "MMM01";
    case MBC3_TIMER_BATTERY:
    case MBC3_TIMER_RAM_BATTERY:
    case MBC3_:
    case MBC3_RAM:
    case MBC3_RAM_BATTERY:
      return "MBC3";
    case MBC5_:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
      return "MBC5";
    case MBC6_:
      return "MBC6";
    case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
      return "MBC7";
    case POCKET_CAMERA:
      return "Pocket Camera";
    case BANDAI_TAMA5:
      return "Bandai TAMA5";
    case HuC3_:
      return "HuC3";
    case HuC1_RAM_BATTERY:
      return "HuC1";
  }
  return "Unknown";
}

bool MBC::bankTypeImplemented() const {
  switch (bankType) {
    case ROM_ONLY:
    case MBC1_:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
    case MBC2_:
    case MBC2_BATTERY:
    case MBC5_:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
      return true;
    default:
      return false;
  }
}

bool MBC::hasRam() const {
  switch (bankType) {
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
    case MMM01_RAM:
    case MMM01_RAM_BATTERY:
    case MBC3_TIMER_RAM_BATTERY:
    case MBC3_RAM:
    case MBC3_RAM_BATTERY:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
    case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
    case HuC1_RAM_BATTERY:
      return true;
    default:
      return false;
  }
}

bool MBC::hasRamAndBattery() const {
  switch (bankType) {
    case MBC1_RAM_BATTERY:
    case MMM01_RAM_BATTERY:
    case MBC3_TIMER_RAM_BATTERY:
    case MBC3_RAM_BATTERY:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE_RAM_BATTERY:
    case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
    case HuC1_RAM_BATTERY:
      return true;
    default:
      return false;
  }
}

int MBC::ramBytes() const {
  switch (ramSize) {
    case RAM_SIZE_8KB:
      return RAM_BANK_BYTES;
    case RAM_SIZE_32KB:
      return RAM_BANK_BYTES * 4;
    case RAM_SIZE_64KB:
      return RAM_BANK_BYTES * 8;
    case RAM_SIZE_128KB:
      return RAM_BANK_BYTES * 16;
  }
  return 0;
}

void MBC::reset() {
  ramEnabled = false;
  romBankNum = 1;
  ramBankNum = 0;
  bankMode = false;
}
