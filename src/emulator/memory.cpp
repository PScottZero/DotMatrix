// **************************************************
// **************************************************
// **************************************************
// MEMORY
// **************************************************
// **************************************************
// **************************************************

#include "memory.h"

#include "bootstrap.h"
#include "cgb.h"
#include "cyclecounter.h"
#include "interrupts.h"
#include "mbc.h"
#include "timers.h"

// allocate space for internal memory, cartridge rom,
// and external ram
uint8 *Memory::mem = (uint8 *)malloc(MEM_BYTES);
uint8 *Memory::cart = (uint8 *)malloc(CART_BYTES);
uint8 *Memory::exram = (uint8 *)malloc(RAM_BANK_BYTES * MAX_RAM_BANKS);

// set initial rom banks and external ram bank
uint8 *Memory::romBank0 = &mem[0];
uint8 *Memory::romBank1 = &mem[ROM_BANK_BYTES];
uint8 *Memory::exramBank = &exram[0];

// **************************************************
// **************************************************
// Read + Write Functions
// **************************************************
// **************************************************

// read 8-bit value from given memory address
uint8 Memory::read(uint16 addr) {
  CycleCounter::addCycles(1);

  // cannot access external ram if it
  // is not enabled by MBC
  if (addr >= EXT_RAM_ADDR && addr < WORK_RAM_ADDR && !MBC::ramEnabled) {
    return 0xFF;
  }

  // do not read value if reading from
  // write-only memory register
  if (addr == NR13 || addr == NR23 || addr == NR31 || addr == NR33 ||
      addr == NR41 || addr == HDMA1 || addr == HDMA2 || addr == HDMA3 ||
      addr == HDMA4) {
    return 0xFF;
  }

  // only the upper 2-bits of register
  // NR11 and NR21 can be read
  if (addr == NR11 || addr == NR21) return readBits(addr, {6, 7});

  // only bit 6 of register NR14,
  // NR24, NR34 and NR44 can be read
  if (addr == NR14 || addr == NR24 || addr == NR34 || addr == NR44) {
    return readBits(addr, {6});
  }

  // skip waiting for screen frame in
  // bootstrap by returning hex 90 when
  // reading the LY register
  if (addr == LY && Bootstrap::enabledAndShouldSkip()) return 0x90;

  // read from memory
  return getByte(addr);
}

// read 16-bit value from given memory address
uint16 Memory::read16(uint16 addr) {
  return read(addr) | (read(addr + 1) << 8);
}

// only read the specified bits of the given
// 8-bit value at the given memory address,
// all other bits are set to 1
uint8 Memory::readBits(uint16 addr, vector<uint8> bits) {
  uint8 mask = 0xFF;
  for (uint8 bit : bits) mask ^= (uint8)pow(2, bit);
  return getByte(addr) | mask;
}

// write 8-bit value to given memory address
void Memory::write(uint16 addr, uint8 val) {
  CycleCounter::addCycles(1);

  // check for MBC writes
  MBC::write(addr, val);

  // rom bank area is read-only
  if (addr < VRAM_ADDR) return;

  // external memory write
  if (addr >= EXT_RAM_ADDR && addr < WORK_RAM_ADDR) {
    // cannot access external ram if it
    // is not enabled by MBC
    if (!MBC::ramEnabled) return;

    // if using MBC2, ram only uses lower nibble
    // of each ram byte, and ram only consists of
    // 512 half bytes with rest of ram area
    // echoing A000-A1FF
    if (MBC::halfRAMMode) {
      echoHalfRam(addr, val);
      return;
    }
  }

  // do not write value if writing to
  // read-only memory register
  if (addr == LY || addr == PCM12 || addr == PCM34) return;

  // only bits 4 and 5 of register P1
  // can be written to
  if (addr == P1) {
    writeBits(addr, val, {4, 5});
  }

  // writing anything to DIV register
  // will reset the internal counter
  // and the DIV register
  if (addr == DIV) {
    Timers::reset();
    return;
  }

  // only bit 7 of register NR52 can
  // be written to
  if (addr == NR52) {
    writeBits(addr, val, {7});
    return;
  }

  // only bits 3-6 of the STAT register
  // can be written to
  if (addr == STAT) {
    writeBits(addr, val, {3, 4, 5, 6});
    return;
  }

  // only bit 0 of register KEY1 can
  // be written to
  if (addr == KEY1) {
    writeBits(addr, val, {0});
    return;
  }

  // can write to all bits of register RP
  // except for bit 1
  if (addr == RP) {
    writeBits(addr, val, {0, 2, 3, 4, 5, 6, 7});
    return;
  }

  // only lower 5 bits of interrupt flag
  // register can be written to (top 3
  // bits are always 1)
  if (addr == IF) {
    getByte(addr) = val | 0xE0;
    return;
  }

  // echo ram
  if (addr >= WORK_RAM_ADDR && addr < OAM_ADDR) {
    echoRam(addr, val);
    return;
  }

  // write to memory
  getByte(addr) = val;

  // start dma transfer if dma
  // address was written to
  if (addr == DMA) dmaTransfer();

  // turn off boostrap if writing
  // nonzero value to address ff50
  if (addr == BOOTSTRAP && val != 0) Bootstrap::enabled = false;

  // start serial transfer if writing
  // to register SC with bit 7 and bit
  // 0 being set to 1
  if (addr == SC && (val & 0x81) == 0x81) {
    CycleCounter::serialTransferMode = true;
  }
}

// write 16-bit value to given memory address
void Memory::write(uint16 addr, uint16 val) {
  write(addr, (uint8)(val & 0xFF));
  write(addr + 1, (uint8)((val >> 8) & 0xFF));
}

// only write the specified bits of the given
// 8-bit value to the given memory address
void Memory::writeBits(uint16 addr, uint16 val, vector<uint8> bits) {
  uint8 mask = 0x00;
  for (uint8 bit : bits) mask |= (uint8)pow(2, bit);
  getByte(addr) = (getByte(addr) & ~mask) | (val & mask);
}

// get 8-bit immediate value
uint8 Memory::imm8(uint16 &PC) { return read(PC++); }

// get 16-bit immediate value
uint16 Memory::imm16(uint16 &PC) {
  uint16 val = read16(PC++);
  ++PC;
  return val;
}

// get byte at specified address directly
// (no read/write intercepts)
uint8 &Memory::getByte(uint16 addr) {
  if (Bootstrap::enabled && addr < BOOTSTRAP_BYTES) {
    return Bootstrap::at(addr);
  } else if (addr < ROM_BANK_1_ADDR) {
    return romBank0[addr];
  } else if (addr < VRAM_ADDR) {
    return romBank1[addr - ROM_BANK_1_ADDR];
  } else if (addr >= EXT_RAM_ADDR && addr < WORK_RAM_ADDR) {
    return exramBank[addr - EXT_RAM_ADDR];
  }
  return mem[addr - VRAM_ADDR];
}

// **************************************************
// **************************************************
// Miscellaneous Functions
// **************************************************
// **************************************************

// load rom at the given directory into memory
void Memory::loadRom(QString dir) {
  fstream fs(dir.toStdString());
  fs.read((char *)cart, CART_BYTES);
  fs.close();

  setRomBank(&romBank0, 0);
  setRomBank(&romBank1, 1);
  setExramBank(0);
}

// map memory rom bank to cartridge rom bank
void Memory::setRomBank(uint8 **romBank, uint8 bankNum) {
  *romBank = &cart[ROM_BANK_BYTES * bankNum];
}

// map memory external ram bank to external ram bank
void Memory::setExramBank(uint8 bankNum) {
  exramBank = &exram[RAM_BANK_BYTES * bankNum];
}

// perform dma transfer
void Memory::dmaTransfer() {
  uint16 dmaAddr = getByte(DMA) << 8;
  for (int i = 0; i < _OAM_ENTRY_COUNT * _OAM_ENTRY_BYTES; ++i) {
    getByte(OAM_ADDR + i) = getByte(dmaAddr + i);
  }
}

// echo memory range C000-DDFF in memory
// range E000-FDFF
void Memory::echoRam(uint16 addr, uint8 val) {
  uint16 offset = addr % RAM_BANK_BYTES;
  getByte(WORK_RAM_ADDR + offset) = val;
  if (ECHO_RAM_ADDR + offset < OAM_ADDR) {
    getByte(ECHO_RAM_ADDR + offset) = val;
  }
}

// echo half-ram memory range A000-A1FF in the
// 15 equal size memory ranges in the rest of
// of the external ram bank
void Memory::echoHalfRam(uint16 addr, uint8 val) {
  uint16 offset = addr % HALF_RAM_BYTES;
  for (int echoAddr = EXT_RAM_ADDR; echoAddr < WORK_RAM_ADDR;
       echoAddr += HALF_RAM_BYTES) {
    getByte(echoAddr + offset) = val | 0xF0;
  }
}

// load external ram, should have same name
// and be in the same directory as the rom,
// except with a .sav extension
void Memory::loadExram() {
  string exramPath = CGB::romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::in);
  if (!fs.fail()) fs.read((char *)exram, MBC::ramBytes());
}

// save external ram, will be saved in the same
// directory as the rom and have the same name
// except with a .sav extension
void Memory::saveExram() {
  string exramPath = CGB::romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::out);
  fs.write((char *)exram, MBC::ramBytes());
}

// reset memory
void Memory::reset() {
  // clear memory
  for (int i = 0; i < MEM_BYTES; ++i) {
    mem[i] = 0;
  }

  // save external ram if current game
  // has ram and battery
  if (MBC::hasRamAndBattery()) saveExram();

  // clear external ram
  for (int i = 0; i < RAM_BANK_BYTES * MAX_RAM_BANKS; ++i) {
    exram[i] = 0;
  }

  // set upper three bits of interrupt
  // flag register
  Interrupts::intFlags = 0xE0;

  // reset rom and ram banks
  setRomBank(&romBank0, 0);
  setRomBank(&romBank1, 1);
  setExramBank(0);
}
