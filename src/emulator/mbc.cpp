#include "mbc.h"

uint8 MBC::bankType = ROM_ONLY;
uint8 MBC::cartSize = CART_SIZE_32KB;

bool MBC::ramEnabled = false;
bool MBC::bankMode = false;
uint8 MBC::romBankNum = 1;
uint8 MBC::ramBankNum = 0;

#include "memory.h"

void MBC::processMBCRequest(uint16 addr, uint8 val) {
  if (bankType == MBC1) {
    // RAM enable register
    if (addr <= RAM_ENABLE) {
      if ((val & 0x0F) == 0x0A) {
        ramEnabled = true;
      } else if (val == 0x00) {
        ramEnabled = false;
      }
    }

    // rom bank number register
    else if (addr <= ROM_BANK_NUM) {
      romBankNum = val & 0x1F;
      if (romBankNum == 0) romBankNum = 1;
      Memory::mapCartMem(Memory::romBank1, romBank1BankNum());
    }

    // ram bank number register
    else if (addr <= RAM_BANK_NUM) {
      ramBankNum = val & 0b11;
      Memory::mapEXRAM(exramBankNum());
    }

    // bank mode select register
    else if (addr <= BANK_MODE) {
      bankMode = val & 0b1;
      Memory::mapCartMem(Memory::romBank0, romBank0BankNum());
      Memory::mapEXRAM(exramBankNum());
    }
  }
}

string MBC::bankTypeStr() {
  switch (bankType) {
    case ROM_ONLY:
      return "ROM Only";
    case MBC1:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
      return "MBC1";
    case MBC2:
    case MBC2_BATTERY:
      return "MBC2";
    case MMM01:
    case MMM01_RAM:
    case MMM01_RAM_BATTERY:
      return "MMM01";
    case MBC3_TIMER_BATTERY:
    case MBC3_TIMER_RAM_BATTERY:
    case MBC3:
    case MBC3_RAM:
    case MBC3_RAM_BATTERY:
      return "MBC3";
    case MBC5:
    case MBC5_RAM:
    case MBC5_RAM_BATTERY:
    case MBC5_RUMBLE:
    case MBC5_RUMBLE_RAM:
    case MBC5_RUMBLE_RAM_BATTERY:
      return "MBC5";
    case MBC6:
      return "MBC6";
    case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
      return "MBC7";
    case POCKET_CAMERA:
      return "Pocket Camera";
    case BANDAI_TAMA5:
      return "Bandai TAMA5";
    case HuC3:
      return "HuC3";
    case HuC1_RAM_BATTERY:
      return "HuC1";
  }
  return "";
}

bool MBC::bankTypeImplemented() {
  switch (bankType) {
    case ROM_ONLY:
    case MBC1:
      return true;
    default:
      return false;
  }
}

bool MBC::bankTypeHasRAM() {
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

uint8 MBC::romBank0BankNum() { return bankMode ? ramBankNum << 5 : 0; }

uint8 MBC::romBank1BankNum() {
  uint8 mask = cartSize != 0 ? (pow(2, cartSize - 1) - 1) : 0;
  return ramBankNum << 5 | (romBankNum & mask);
}

uint8 MBC::exramBankNum() { return bankMode ? ramBankNum : 0; }
