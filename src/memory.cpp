#include "memory.h"

#include <fstream>

#include "ppu.h"

Memory::Memory()
    : mem((uint8 *)malloc(MEM_BYTES)),
      cart((uint8 *)malloc(CART_BYTES)),
      romBank0((uint8 **)malloc(sizeof(uint8 *) * ROM_BANK_BYTES)),
      romBank1((uint8 **)malloc(sizeof(uint8 *) * ROM_BANK_BYTES)) {
  init();
}

Memory::~Memory() {
  free(mem);
  free(cart);
  free(romBank0);
  free(romBank1);
}

void Memory::init() {
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

void Memory::loadROM(string dir) {
  fstream fs(dir);
  fs.read((char *)cart, CART_BYTES);
  fs.close();

  mapCartMem(romBank0, 0x000000);
  mapCartMem(romBank1, 0x004000);
}

void Memory::mapCartMem(uint8 **romBank, uint16 startAddr) {
  for (int i = 0; i < ROM_BANK_BYTES; ++i) {
    romBank[i] = &cart[startAddr + i];
  }
}

uint8 Memory::read(uint16 addr, uint8 &cycles) const {
  ++cycles;

  // read from rom bank 0
  if (addr >= ROM_BANK_0_ADDR && addr < ROM_BANK_1_ADDR) {
    return *romBank0[addr];
  }

  // read from rom bank 1
  else if (addr >= ROM_BANK_1_ADDR && addr < VRAM_ADDR) {
    return *romBank1[addr - ROM_BANK_BYTES];
  }

  // read from memory
  else if (addr >= VRAM_ADDR && addr < RAM_ADDR && canAccessVRAM() ||
           addr >= RAM_ADDR && addr < OAM_ADDR ||
           addr >= OAM_ADDR && addr <= OAM_END_ADDR && canAccessOAM() ||
           addr >= ZERO_PAGE_ADDR) {
    return mem[addr - MEM_BYTES];
  }

  return 0xFF;
}

uint16 Memory::read16(uint16 addr, uint8 &cycles) const {
  return read(addr, cycles) | (read(addr + 1, cycles) << 8);
}

void Memory::write(uint16 addr, uint8 val, uint8 &cycles) {
  ++cycles;

  // write to memory
  if (addr >= VRAM_ADDR && addr < RAM_ADDR && canAccessVRAM() ||
      addr >= RAM_ECHO_END_ADDR && addr < ECHO_RAM_ADDR ||
      addr >= OAM_ADDR && addr <= OAM_END_ADDR && canAccessOAM() ||
      addr >= ZERO_PAGE_ADDR) {
    mem[addr - MEM_BYTES] = val;
  }

  // write to ram and echo ram
  else if (addr >= RAM_ADDR && addr < OAM_ADDR) {
    uint16 offset = addr - (addr < ECHO_RAM_ADDR ? RAM_ADDR : ECHO_RAM_ADDR);
    uint16 echoAddr =
        (addr >= ECHO_RAM_ADDR ? RAM_ADDR : ECHO_RAM_ADDR) + offset;
    mem[addr - MEM_BYTES] = val;
    mem[echoAddr - MEM_BYTES] = val;
  }
}

void Memory::write(uint16 addr, uint16 val, uint8 &cycles) {
  write(addr, (uint8)(val & 0xFF), cycles);
  write(addr + 1, (uint8)((val >> 8) & 0xFF), cycles);
}

uint8 Memory::imm8(uint16 &PC, uint8 &cycles) const {
  return read(PC++, cycles);
}

uint16 Memory::imm16(uint16 &PC, uint8 &cycles) const {
  uint16 val = read16(PC++, cycles);
  ++PC;
  return val;
}

uint8 Memory::getByte(uint16 addr) const { return mem[addr - MEM_BYTES]; }

uint16 Memory::getTwoBytes(uint16 addr) const {
  return (mem[addr - MEM_BYTES] << 8) | mem[addr - MEM_BYTES + 1];
}

uint8 *Memory::getPtr(uint16 addr) { return &mem[addr - MEM_BYTES]; }

uint8 &Memory::getRef(uint16 addr) { return mem[addr - MEM_BYTES]; }

bool Memory::canAccessVRAM() const {
  return (mem[STAT] & 0b11) != PIXEL_TRANSFER_MODE;
}

bool Memory::canAccessOAM() const {
  return (mem[STAT] & 0b11) != PIXEL_TRANSFER_MODE &&
         (mem[STAT] & 0b11) != OAM_SEARCH_MODE;
}
