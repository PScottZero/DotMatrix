#include "memory.h"

Memory::Memory()
    : mem((uint8 *)malloc(MEM_BYTES)),
      cart((uint8 *)malloc(CART_BYTES)),
      romBank0((uint8 **)malloc(sizeof(uint8 *) * ROM_BANK_BYTES)),
      romBank1((uint8 **)malloc(sizeof(uint8 *) * ROM_BANK_BYTES)),
      dmaTransferMode(false) {}

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

  // can only read from HRAM during dma transfer
  if (!dmaTransferMode || (addr >= HRAM_ADDR)) {
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
      return getByte(addr);
    }
  }

  return 0x00;
}

// read 16-bit value from given memory address
uint16 Memory::read16(uint16 addr, uint8 &cycles) {
  return read(addr, cycles) | (read(addr + 1, cycles) << 8);
}

// write 8-bit value to given memory address
void Memory::write(uint16 addr, uint8 val, uint8 &cycles) {
  ++cycles;

  // can only write to HRAM during dma transfer
  if (!dmaTransferMode || (addr >= HRAM_ADDR)) {
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
    else if (addr >= VRAM_ADDR && addr < RAM_ADDR && canAccessVRAM() ||
             addr >= RAM_ECHO_END_ADDR && addr < ECHO_RAM_ADDR ||
             addr >= OAM_ADDR && addr <= OAM_END_ADDR && canAccessOAM() ||
             addr >= ZERO_PAGE_ADDR) {
      setByte(addr, val);
    }

    // write to ram and echo ram
    else if (addr >= RAM_ADDR && addr < OAM_ADDR) {
      uint16 offset = addr - (addr < ECHO_RAM_ADDR ? RAM_ADDR : ECHO_RAM_ADDR);
      uint16 echoAddr =
          (addr >= ECHO_RAM_ADDR ? RAM_ADDR : ECHO_RAM_ADDR) + offset;
      setByte(addr, val);
      setByte(echoAddr, val);
    }
  }

  // start dma transfer if dma
  // address was written to
  if (addr == DMA) {
    std::thread dmaTransferThread(&Memory::dmaTransfer, this);
    dmaTransferThread.detach();
  }
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

uint8 &Memory::getByte(uint16 addr) { return mem[addr - MEM_BYTES]; }

uint8 *Memory::getBytePtr(uint16 addr) { return &mem[addr - MEM_BYTES]; }

void Memory::setByte(uint16 addr, uint8 val) { mem[addr - MEM_BYTES] = val; }

uint16 Memory::getTwoBytes(uint16 addr) {
  return (getByte(addr) << 8) | getByte(addr + 1);
}

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

// map cartridge memory block to rgiven om bank
void Memory::mapCartMem(uint8 **romBank, uint16 startAddr) {
  for (int i = 0; i < ROM_BANK_BYTES; ++i) {
    romBank[i] = &cart[startAddr + i];
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
