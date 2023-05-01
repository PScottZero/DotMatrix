#include "memory.h"

#include "bootstrap.h"
#include "clock.h"
#include "controls.h"
#include "json.hpp"

Memory::Memory()
    : mem((uint8 *)malloc(MEM_BYTES)),
      cart((uint8 *)malloc(CART_BYTES)),
      romBank0((uint8 *)malloc(ROM_BANK_BYTES)),
      romBank1((uint8 *)malloc(ROM_BANK_BYTES)),
      dmaTransferMode(false) {}

Memory::~Memory() {
  free(mem);
  free(cart);
  free(romBank0);
  free(romBank1);
}

// **************************************************
// **************************************************
// Read + Write Functions
// **************************************************
// **************************************************

// read 8-bit value from given memory address
uint8 Memory::read(uint16 addr, uint8 &cycles) {
  ++cycles;

  // return zero if reading from a
  // restricted memory location
  if (memoryRestricted(addr)) {
    return 0x00;
  }

  // skip waiting for screen frame in
  // bootstrap by returning hex 90 when
  // reading the LY register
  if (addr == LY && Bootstrap::enabled && Bootstrap::skip) {
    return 0x90;
  }

  // read from memory
  return getByte(addr);
}

// read 16-bit value from given memory address
uint16 Memory::read16(uint16 addr, uint8 &cycles) {
  return read(addr, cycles) | (read(addr + 1, cycles) << 8);
}

// write 8-bit value to given memory address
void Memory::write(uint16 addr, uint8 val, uint8 &cycles) {
  ++cycles;

  // return if writing to a
  // restricted memory location
  if (memoryRestricted(addr) || addr < VRAM_ADDR) {
    return;
  }

  // writing anything to DIV register
  // will set its value to zero
  if (addr == DIV) {
    setByte(DIV, 0);
  }

  // only bits 6-3 of the STAT register
  // can be written to
  else if (addr == STAT) {
    setByte(STAT, (getByte(STAT) & 0x87) | (val & 0x78));
  }

  // LY register is read-only
  else if (addr == LY) {
    return;
  }

  // turn off boostrap if writing
  // nonzero value to address ff50
  else if (addr == BOOTSTRAP && val != 0) {
    Bootstrap::enabled = false;
    if (Bootstrap::skip) Clock::reset();
  }

  // write to memory
  setByte(addr, val);

  // start dma transfer if dma
  // address was written to
  if (addr == DMA) {
    thread dmaTransferThread(&Memory::dmaTransfer, this);
    dmaTransferThread.detach();
  }

  // update state of controls if
  // writing to joypad register
  if (addr == P1) {
    Controls::update();
  }
}

// check if specified addr is a restricted
// memory location
bool Memory::memoryRestricted(uint16 addr) {
  // can only access HRAM during
  // DMA transfers
  if (dmaTransferMode && addr < HRAM_ADDR) {
    return true;
  }

  // cannot access OAM if lcd is in
  // pixel transfer or oam search mode
  if (addr >= OAM_ADDR && addr <= OAM_END_ADDR && !canAccessOAM()) {
    return true;
  }

  // cannot access VRAM if lcd is in
  // pixel transfer mode
  if (addr >= VRAM_ADDR && addr < RAM_ADDR && !canAccessVRAM()) {
    return true;
  }

  return false;
}

// write 16-bit value to given memory address
void Memory::write(uint16 addr, uint16 val, uint8 &cycles) {
  write(addr, (uint8)(val & 0xFF), cycles);
  write(addr + 1, (uint8)((val >> 8) & 0xFF), cycles);
}

// get 8-bit immediate value
uint8 Memory::imm8(uint16 &PC, uint8 &cycles) { return read(PC++, cycles); }

// get 16-bit immediate value
uint16 Memory::imm16(uint16 &PC, uint8 &cycles) {
  uint16 val = read16(PC++, cycles);
  ++PC;
  return val;
}

// **************************************************
// **************************************************
// Direct Memory Access Functions
// **************************************************
// **************************************************

uint8 *Memory::getBytePtr(uint16 addr) {
  if (Bootstrap::enabled && addr < BOOTSTRAP_BYTES) {
    return Bootstrap::at(addr);
  } else if (addr < ROM_BANK_1_ADDR) {
    return &romBank0[addr];
  } else if (addr < VRAM_ADDR) {
    return &romBank1[addr - ROM_BANK_BYTES];
  }
  return &mem[addr - MEM_BYTES];
}

uint8 &Memory::getByte(uint16 addr) { return *getBytePtr(addr); }

void Memory::setByte(uint16 addr, uint8 val) { *getBytePtr(addr) = val; }

// check if VRAM can be accessed,
// can only be access outside of
// pixel transfer mode
bool Memory::canAccessVRAM() {
  return (getByte(STAT) & 0b11) != _PIXEL_TRANSFER_MODE;
}

// check if OAM memory can be accessed,
// can only be accesses outside of
// pixel transfer and oam search mode
bool Memory::canAccessOAM() {
  return (getByte(STAT) & 0b11) != _PIXEL_TRANSFER_MODE &&
         (getByte(STAT) & 0b11) != _OAM_SEARCH_MODE;
}

// **************************************************
// **************************************************
// Miscellaneous Functions
// **************************************************
// **************************************************

// load rom at the given directory into memory
void Memory::loadROM(QString dir) {
  fstream fs(dir.toStdString());
  fs.read((char *)cart, CART_BYTES);
  fs.close();

  mapCartMem(romBank0, 0x000000);
  mapCartMem(romBank1, 0x004000);
}

// map cartridge memory block to rgiven om bank
void Memory::mapCartMem(uint8 *romBank, uint16 startAddr) {
  for (int i = 0; i < ROM_BANK_BYTES; ++i) {
    romBank[i] = cart[startAddr + i];
  }
}

// perform dma transfer
void Memory::dmaTransfer() {
  dmaTransferMode = true;
  uint16 dmaAddr = getByte(DMA) << 8;
  for (int i = 0; i < _OAM_ENTRY_COUNT * _OAM_ENTRY_BYTES; ++i) {
    setByte(OAM_ADDR + i, getByte(dmaAddr + i));
  }
  dmaTransferMode = false;
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
  for (int i = 0; i < MEM_BYTES; ++i) {
    mem[i] = 0;
  }
  dmaTransferMode = false;
  mapCartMem(romBank0, 0x000000);
  mapCartMem(romBank1, 0x004000);
}
