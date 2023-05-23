// **************************************************
// **************************************************
// **************************************************
// Memory Bank Controller (MBC)
// **************************************************
// **************************************************
// **************************************************

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

// mbc registers
#define MBC1_RAM_ENABLE 0x1FFF
#define MBC1_ROM_BANK_NUM 0x3FFF
#define MBC1_RAM_BANK_NUM 0x5FFF
#define MBC1_BANK_MODE 0x7FFF
#define MBC2_RAM_ROM 0x3FFF
#define MBC3_EXRAM_RTC 0xBFFF
#define MBC3_RAM_TIMER_ENABLE 0x1FFF
#define MBC3_ROM_BANK_NUM 0x3FFF
#define MBC3_RAM_BANK_NUM_RTC_SELECT 0x5FFF
#define MBC3_LATCH_CLOCK_DATA 0x7FFF
#define MBC5_RAM_ENABLE 0x1FFF
#define MBC5_ROM_BANK_BYTE_LO 0x2FFF
#define MBC5_ROM_BANK_BIT_9 0x3FFF
#define MBC5_RAM_BANK_NUM 0x5FFF

class Memory;
class RTC;

using namespace std;

class MBC {
 private:
 public:
  Memory *mem;
  RTC *rtc;

  uint8 bankType, romSize, ramSize, romBankNum, ramBankNum;
  bool ramEnabled, bankMode, halfRAMMode, romBankBit9;

  MBC();

  // mbc functions
  void write(uint16 addr, uint8 val);
  void mbc1(uint16 addr, uint8 val);
  void mbc2(uint16 addr, uint8 val);
  void mbc3(uint16 addr, uint8 val);
  void mbc5(uint16 addr, uint8 val);

  // mbc helper functions
  uint8 mbc1RomBank0BankNum() const;
  uint8 mbc1RomBank1BankNum() const;
  uint8 mbc1ExramBankNum() const;
  uint16 mbc5RomBankNum() const;
  uint8 mbc5RamBankNum() const;
  bool usingMbc1() const;
  bool usingMbc2() const;
  bool usingMbc3() const;
  bool usingMbc5() const;

  // mbc config functions
  uint16 romSizeMask() const;
  uint8 ramSizeMask() const;
  bool bankTypeImplemented() const;
  string bankTypeStr() const;
  bool hasRam() const;
  bool hasRamAndBattery() const;
  bool hasTimerAndBattery() const;
  int ramBytes() const;
  void reset();
};
