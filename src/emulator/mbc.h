#pragma once

#include <map>
#include <string>

#include "types.h"

// bank types
#define ROM_ONLY 0x00
#define MBC1 0x01
#define MBC1_RAM 0x02
#define MBC1_RAM_BATTERY 0x03
#define MBC2 0x05
#define MBC2_BATTERY 0x06
#define MMM01 0x0B
#define MMM01_RAM 0x0C
#define MMM01_RAM_BATTERY 0x0D
#define MBC3_TIMER_BATTERY 0x0F
#define MBC3_TIMER_RAM_BATTERY 0x10
#define MBC3 0x11
#define MBC3_RAM 0x12
#define MBC3_RAM_BATTERY 0x13
#define MBC5 0x19
#define MBC5_RAM 0x1A
#define MBC5_RAM_BATTERY 0x1B
#define MBC5_RUMBLE 0x1C
#define MBC5_RUMBLE_RAM 0x1D
#define MBC5_RUMBLE_RAM_BATTERY 0x1E
#define MBC6 0x20
#define MBC7_SENSOR_RUMBLE_RAM_BATTERY 0x22
#define POCKET_CAMERA 0xFC
#define BANDAI_TAMA5 0xFD
#define HuC3 0xFE
#define HuC1_RAM_BATTERY 0xFF

// cart sizes
#define CART_SIZE_32KB 0x00
#define CART_SIZE_64KB 0x01
#define CART_SIZE_128KB 0x02
#define CART_SIZE_256KB 0x03
#define CART_SIZE_512KB 0x04
#define CART_SIZE_1MB 0x05
#define CART_SIZE_2MB 0x06
#define CART_SIZE_4MB 0x07
#define CART_SIZE_8MB 0x08

// bank register addresses
#define RAM_ENABLE 0x1FFF
#define ROM_BANK_NUM 0x3FFF
#define RAM_BANK_NUM 0x5FFF
#define BANK_MODE 0x7FFF

using namespace std;

class MBC {
 private:
  static bool bankMode;
  static uint8 romBankNum;
  static uint8 ramBankNum;
  static bool bankTypeHasRAM();

 public:
  static bool ramEnabled;
  static uint8 bankType;
  static uint8 cartSize;

  static void processMBCRequest(uint16 addr, uint8 val);
  static bool bankTypeImplemented();
  static string bankTypeStr();

  static uint8 romBank0BankNum();
  static uint8 romBank1BankNum();
  static uint8 exramBankNum();
};
