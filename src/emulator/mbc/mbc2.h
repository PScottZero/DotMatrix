#pragma once

#include "../types.h"

#define RAM_ENABLE 0x1FFF
#define ROM_BANK_NUM 0x3FFF
#define RAM_BANK_NUM 0x5FFF
#define BANK_MODE 0x7FFF

#define RAM_ENABLE_ROM_BANK_NUM 0x3FFF

class MBC2 {
 private:
  static uint8 romBankNum;

 public:
  static void write(uint16 addr, uint8 val);
  static void reset();
};
