#include "mbc.h"

#include <math.h>

#include "mbc1.h"
#include "mbc2.h"

uint8 MBC::bankType = ROM_ONLY;
uint8 MBC::cartSize = CART_SIZE_32KB;

bool MBC::ramEnabled = false;

void MBC::write(uint16 addr, uint8 val) {
  switch (bankType) {
    case MBC1_:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
      MBC1::write(addr, val);
      break;
    case MBC2_:
    case MBC2_BATTERY:
      MBC2::write(addr, val);
      break;
  }
}

string MBC::bankTypeStr() {
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

bool MBC::bankTypeImplemented() {
  switch (bankType) {
    case ROM_ONLY:
    case MBC1_:
    case MBC1_RAM:
    case MBC1_RAM_BATTERY:
    case MBC2_:
    case MBC2_BATTERY:
      return true;
    default:
      return false;
  }
}

bool MBC::hasRAM() {
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

void MBC::reset() {
  ramEnabled = false;
  MBC1::reset();
  MBC2::reset();
}
