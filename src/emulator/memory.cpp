#include "memory.h"

#include "json.hpp"

Memory::Memory()
    : mem((uint8 *)malloc(MEM_BYTES)),
      cart((uint8 *)malloc(CART_BYTES)),
      romBank0((uint8 *)malloc(ROM_BANK_BYTES)),
      romBank1((uint8 *)malloc(ROM_BANK_BYTES)),
      dmaTransferMode(false),
      controls(nullptr) {}

Memory::~Memory() {
  free(mem);
  free(cart);
  free(romBank0);
  free(romBank1);
}

// initialize memory
void Memory::init() {
  mem[P1] = 0xFF;

  mem[TIMA] = 0x00;
  mem[TMA] = 0x00;
  mem[TAC] = 0x00;
  mem[NR10] = 0x80;
  mem[NR11] = 0xBF;
  mem[NR12] = 0xF3;
  mem[NR14] = 0xBF;
  mem[NR21] = 0x3F;
  mem[NR22] = 0x00;
  mem[NR24] = 0xBF;
  mem[NR30] = 0x7F;
  mem[NR31] = 0xFF;
  mem[NR32] = 0x9F;
  mem[NR34] = 0xBF;
  mem[NR41] = 0xFF;
  mem[NR42] = 0x00;
  mem[NR43] = 0x00;
  mem[NR44] = 0xBF;
  mem[NR50] = 0x77;
  mem[NR51] = 0xF3;
  mem[NR52] = 0xF1;
  mem[LCDC] = 0x91;
  mem[SCY] = 0x00;
  mem[SCX] = 0x00;
  mem[LYC] = 0x00;
  mem[BGP] = 0xFC;
  mem[OBP0] = 0xFF;
  mem[OBP1] = 0xFF;
  mem[WY] = 0x00;
  mem[WX] = 0x00;
  mem[IE] = 0x00;
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

  // write to memory
  setByte(addr, val);

  // start dma transfer if dma
  // address was written to
  if (addr == DMA) {
    std::thread dmaTransferThread(&Memory::dmaTransfer, this);
    dmaTransferThread.detach();
  }

  // update state of controls if
  // writing to joypad register
  if (addr == P1) {
    if (controls != nullptr) {
      controls->update();
    }
  }
}

// check if specified addr is a restricted
// memory location
bool Memory::memoryRestricted(uint16_t addr) {
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
  if (addr < ROM_BANK_1_ADDR) {
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
  return (mem[STAT] & 0b11) != _PIXEL_TRANSFER_MODE;
}

// check if OAM memory can be accessed,
// can only be accesses outside of
// pixel transfer and oam search mode
bool Memory::canAccessOAM() {
  return (mem[STAT] & 0b11) != _PIXEL_TRANSFER_MODE &&
         (mem[STAT] & 0b11) != _OAM_SEARCH_MODE;
}

// **************************************************
// **************************************************
// Miscellaneous Functions
// **************************************************
// **************************************************

// load rom at the given directory into memory
void Memory::loadROM(QString dir) {
  std::fstream fs(dir.toStdString());
  fs.read((char *)cart, CART_BYTES);
  fs.close();

  mapCartMem(romBank0, 0x000000);
  mapCartMem(romBank1, 0x004000);
}

void Memory::loadNintendoLogo() {
  const std::vector<uint8> logoData = {
      0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
      0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
      0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63,
      0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};
  for (int i = 0; i < logoData.size(); ++i) {
    romBank0[0x0104 + i] = logoData[i];
  }
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
  uint16 dmaAddr = mem[DMA] * 0x100;
  for (int i = 0; i < _OAM_ENTRY_COUNT * _OAM_ENTRY_BYTES; ++i) {
    mem[OAM_ADDR + i] = mem[dmaAddr + i];
  }
  dmaTransferMode = false;
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
