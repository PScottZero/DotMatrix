#pragma once

#include <map>
#include <string>

#include "types.h"

// bank types
#define ROM_ONLY 0x00
#define MBC1_ 0x01
#define MBC1_RAM 0x02
#define MBC1_RAM_BATTERY 0x03
#define MBC2_ 0x05
#define MBC2_BATTERY 0x06
#define MMM01_ 0x0B
#define MMM01_RAM 0x0C
#define MMM01_RAM_BATTERY 0x0D
#define MBC3_TIMER_BATTERY 0x0F
#define MBC3_TIMER_RAM_BATTERY 0x10
#define MBC3_ 0x11
#define MBC3_RAM 0x12
#define MBC3_RAM_BATTERY 0x13
#define MBC5_ 0x19
#define MBC5_RAM 0x1A
#define MBC5_RAM_BATTERY 0x1B
#define MBC5_RUMBLE 0x1C
#define MBC5_RUMBLE_RAM 0x1D
#define MBC5_RUMBLE_RAM_BATTERY 0x1E
#define MBC6_ 0x20
#define MBC7_SENSOR_RUMBLE_RAM_BATTERY 0x22
#define POCKET_CAMERA 0xFC
#define BANDAI_TAMA5 0xFD
#define HuC3_ 0xFE
#define HuC1_RAM_BATTERY 0xFF

// rom sizes
#define ROM_SIZE_32KB 0x00
#define ROM_SIZE_64KB 0x01
#define ROM_SIZE_128KB 0x02
#define ROM_SIZE_256KB 0x03
#define ROM_SIZE_512KB 0x04
#define ROM_SIZE_1MB 0x05
#define ROM_SIZE_2MB 0x06
#define ROM_SIZE_4MB 0x07
#define ROM_SIZE_8MB 0x08

// ram sizes
#define NO_RAM 0x00
#define RAM_SIZE_8KB 0x02
#define RAM_SIZE_32KB 0x03
#define RAM_SIZE_128KB 0x04
#define RAM_SIZE_64KB 0x05

// mbc1 register constants
#define MBC1_RAM_ENABLE 0x1FFF
#define MBC1_ROM_BANK_NUM 0x3FFF
#define MBC1_RAM_BANK_NUM 0x5FFF
#define MBC1_BANK_MODE 0x7FFF

// mbc2 register constants
#define MBC2_RAM_ROM 0x3FFF

using namespace std;

class MBC {
 private:
 public:
  static uint8 bankType;
  static uint8 romSize;
  static uint8 ramSize;

  static bool ramEnabled, bankMode;
  static uint8 romBankNum, ramBankNum;

  static bool halfRAMMode;

  static void write(uint16 addr, uint8 val);

  // mbc1 functions
  static void mbc1(uint16 addr, uint8 val);
  static uint8 mbc1ROMBank0BankNum();
  static uint8 mbc1ROMBank1BankNum();
  static uint8 mbc1EXRAMBankNum();

  // mbc2 functions
  static void mbc2(uint16 addr, uint8 val);

  static uint8 romSizeMask();
  static uint8 ramSizeMask();
  static bool bankTypeImplemented();
  static string bankTypeStr();
  static bool hasRAM();
  static int ramBytes();
  static void reset();
};
