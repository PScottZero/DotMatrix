#pragma once

#include "../types.h"

#define RAM_ENABLE 0x1FFF
#define ROM_BANK_NUM 0x3FFF
#define RAM_BANK_NUM 0x5FFF
#define BANK_MODE 0x7FFF

class MBC1 {
 private:
  static bool bankMode;
  static uint8 romBankNum;
  static uint8 ramBankNum;

  static uint8 romBank0BankNum();
  static uint8 romBank1BankNum();
  static uint8 exramBankNum();
  static uint8 hasRAM();

 public:
  static void write(uint16 addr, uint8 val);
  static void reset();
};
