#include "memory.h"

#include "bootstrap.h"
#include "cgb.h"
#include "controls.h"
#include "cyclecounter.h"
#include "interrupts.h"
#include "mbc.h"
#include "timers.h"

uint8 *Memory::mem = (uint8 *)malloc(MEM_BYTES);
uint8 *Memory::cart = (uint8 *)malloc(CART_BYTES);
uint8 *Memory::exram = (uint8 *)malloc(RAM_BANK_BYTES * MAX_RAM_BANKS);
uint8 *Memory::romBank0 = (uint8 *)malloc(ROM_BANK_BYTES);
uint8 *Memory::romBank1 = (uint8 *)malloc(ROM_BANK_BYTES);
uint8 **Memory::exramBank = (uint8 **)malloc(sizeof(uint8 *) * RAM_BANK_BYTES);

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

  // skip waiting for screen frame in
  // bootstrap by returning hex 90 when
  // reading the LY register
  if (addr == LY && Bootstrap::skipWait()) {
    return 0x90;
  }

  // read from memory
  return getByte(addr);
}

// read 16-bit value from given memory address
uint16 Memory::read16(uint16 addr) {
  return read(addr) | (read(addr + 1) << 8);
}

// write 8-bit value to given memory address
void Memory::write(uint16 addr, uint8 val) {
  CycleCounter::addCycles(1);

  // check for MBC writes
  MBC::write(addr, val);

  if (addr < VRAM_ADDR) {
    return;
  }

  // cannot access external ram if it
  // is not enabled by MBC
  if (addr >= EXT_RAM_ADDR && addr < WORK_RAM_ADDR && !MBC::ramEnabled) {
    return;
  }

  // LY register is read-only
  if (addr == LY) {
    return;
  }

  // turn off boostrap if writing
  // nonzero value to address ff50
  if (addr == BOOTSTRAP && val != 0) {
    Bootstrap::enabled = false;
  }

  // writing anything to DIV register
  // will reset the internal counter
  // and the DIV register
  if (addr == DIV) {
    Timers::internalCounter = 0;
    getByte(DIV) = 0;
  }

  // only bits 6-3 of the STAT register
  // can be written to
  else if (addr == STAT) {
    getByte(STAT) = (getByte(STAT) & 0x87) | (val & 0x78);
  }

  // echo ram
  else if (addr >= WORK_RAM_ADDR && addr < OAM_ADDR) {
    echoRam(addr, val);
  }

  // if using MBC2, ram only uses lower nibble
  // of each ram byte, and ram only consists of
  // 512 half bytes with rest of ram area
  // echoing A000-A1FF
  else if (addr >= EXT_RAM_ADDR && addr < WORK_RAM_ADDR && MBC::halfRAMMode) {
    echoHalfRam(addr, val);
  }

  // write to memory
  else {
    getByte(addr) = val;
  }

  // upper 3 bits of IF are always 1
  if (addr == IF) {
    getByte(addr) |= 0xE0;
  }

  // start dma transfer if dma
  // address was written to
  else if (addr == DMA) {
    dmaTransfer();
  }

  // update state of controls if
  // writing to joypad register
  else if (addr == P1) {
    Controls::update();
  }
}

// write 16-bit value to given memory address
void Memory::write(uint16 addr, uint16 val) {
  write(addr, (uint8)(val & 0xFF));
  write(addr + 1, (uint8)((val >> 8) & 0xFF));
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
    return *exramBank[addr - EXT_RAM_ADDR];
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

  setRomBank(romBank0, 0);
  setRomBank(romBank1, 1);
  setExramBank(0);
}

// map memory rom bank to cartridge rom bank
void Memory::setRomBank(uint8 *romBank, uint8 bankNum) {
  uint32 startAddr = ROM_BANK_BYTES * bankNum;
  for (int i = 0; i < ROM_BANK_BYTES; ++i) {
    romBank[i] = cart[startAddr + i];
  }
}

// map memory external ram bank to external ram bank
void Memory::setExramBank(uint8 bankNum) {
  uint16 startAddr = RAM_BANK_BYTES * bankNum;
  for (int i = 0; i < RAM_BANK_BYTES; ++i) {
    exramBank[i] = &exram[startAddr + i];
  }
}

// // map memory rom bank to cartridge rom bank
// void Memory::setRomBank(uint8 *romBank, uint8 bankNum) {
//   romBank = &cart[ROM_BANK_BYTES * bankNum];
// }

// // map memory external ram bank to external ram bank
// void Memory::setExramBank(uint8 bankNum) {
//   exramBank = &exram[RAM_BANK_BYTES * bankNum];
// }

// perform dma transfer
void Memory::dmaTransfer() {
  uint16 dmaAddr = getByte(DMA) << 8;
  for (int i = 0; i < _OAM_ENTRY_COUNT * _OAM_ENTRY_BYTES; ++i) {
    getByte(OAM_ADDR + i) = getByte(dmaAddr + i);
  }
}

void Memory::echoRam(uint16 addr, uint8 val) {
  uint16 offset = addr % RAM_BANK_BYTES;
  getByte(WORK_RAM_ADDR + offset) = val;
  if (ECHO_RAM_ADDR + offset < OAM_ADDR) {
    getByte(ECHO_RAM_ADDR + offset) = val;
  }
}

void Memory::echoHalfRam(uint16 addr, uint8 val) {
  uint16 offset = addr % HALF_RAM_BYTES;
  for (int echoAddr = EXT_RAM_ADDR; echoAddr < WORK_RAM_ADDR;
       echoAddr += HALF_RAM_BYTES) {
    getByte(echoAddr + offset) = val | 0xF0;
  }
}

void Memory::loadExram() {
  string exramPath = CGB::romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::in);
  if (!fs.fail()) fs.read((char *)exram, MBC::ramBytes());
}

void Memory::saveExram() {
  string exramPath = CGB::romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::out);
  fs.write((char *)exram, MBC::ramBytes());
}

void Memory::loadState() {
  fstream fs("/Users/paulscott/git/DotMatrix/debug/memory_state.bin", ios::in);
  fs.read((char *)romBank0, ROM_BANK_BYTES);
  fs.read((char *)romBank1, ROM_BANK_BYTES);
  fs.read((char *)mem, MEM_BYTES);
}

void Memory::saveState() {
  fstream fs("/Users/paulscott/git/DotMatrix/debug/memory_state.bin", ios::out);
  fs.write((char *)romBank0, ROM_BANK_BYTES);
  fs.write((char *)romBank1, ROM_BANK_BYTES);
  fs.write((char *)mem, MEM_BYTES);
  fs.close();
}

void Memory::reset() {
  // clear memory
  for (int i = 0; i < MEM_BYTES; ++i) {
    mem[i] = 0;
  }

  // clear external ram
  for (int i = 0; i < RAM_BANK_BYTES * MAX_RAM_BANKS; ++i) {
    exram[i] = 0;
  }

  // TODO load external ram after clearing

  Interrupts::intFlags |= 0xE0;
  setRomBank(romBank0, 0);
  setRomBank(romBank1, 1);
  setExramBank(0);
}

void Memory::loadState() {
  std::fstream fs("../debug/memory_state.bin", std::ios::in);
  fs.read((char *)romBank0, ROM_BANK_BYTES);
  fs.read((char *)romBank1, ROM_BANK_BYTES);
  fs.read((char *)mem, MEM_BYTES);
}

void Memory::saveState() {
  std::fstream fs("../debug/memory_state.bin", std::ios::out);
  fs.write((char *)romBank0, ROM_BANK_BYTES);
  fs.write((char *)romBank1, ROM_BANK_BYTES);
  fs.write((char *)mem, MEM_BYTES);
  fs.close();
}
