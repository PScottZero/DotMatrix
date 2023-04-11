#pragma once

#include "utils.h"

#define MEM_BYTES 0x8000
#define CART_BYTES 0x800000
#define ROM_BANK_BYTES 0x4000

// dmg in-memory flags
#define P1 0xFF00
#define SB 0xFF01
#define SC 0xFF02
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define IF 0xFF0F
#define IE 0xFFFF
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define DMA 0xFF46
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49
#define WY 0xFF4A
#define WX 0xFF4B
#define OAM_START 0xFE00
#define OAM_END 0xFE9F
#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0xFF12
#define NR13 0xFF13
#define NR14 0xFF14
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1D
#define NR34 0xFF1E
#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR44 0xFF23
#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26
#define WAVEFORM_START 0xFF30
#define WAVEFORM_END 0xFF3F

// cgb in-memory flags
#define KEY1 0xFF4D
#define RP 0xFF56
#define VBK 0xFF4F
#define SVBK 0xFF70
#define HDMA1 0xFF51
#define HDMA2 0xFF52
#define HDMA3 0xFF53
#define HDMA4 0xFF54
#define HDMA5 0xFF55
#define BCPS 0xFF68
#define BCPD 0xFF69
#define OCPS 0xFF6A
#define OCPD 0xFF6B

// memory map
#define ROM_BANK_0_ADDR 0x0000
#define ROM_BANK_1_ADDR 0x4000
#define VRAM_ADDR 0x8000
#define RAM_BANK_ADDR 0xA000
#define RAM_ADDR 0xC000
#define RAM_ECHO_END_ADDR 0xDE00
#define ECHO_RAM_ADDR 0xE000
#define OAM_ADDR 0xFE00
#define OAM_END_ADDR 0xFFA0
#define ZERO_PAGE_ADDR 0xFF00

using namespace std;

class Memory {
 private:
  uint8 *mem;
  uint8 *cart;
  uint8 **romBank0;
  uint8 **romBank1;

  void init();

  void mapCartMem(uint8 **romBank, uint16 startAddr);

  // access functions
  bool canAccessVRAM() const;
  bool canAccessOAM() const;

 public:
  Memory();
  ~Memory();

  void loadROM(string dir);

  // memory read + write functions
  uint8 read(uint16 addr, uint8 &cycles) const;
  uint16 read16(uint16 addr, uint8 &cycles) const;
  void write(uint16 addr, uint8 val, uint8 &cycles);
  void write(uint16 addr, uint16 val, uint8 &cycles);
  uint8 imm8(uint16 &PC, uint8 &cycles) const;
  uint16 imm16(uint16 &PC, uint8 &cycles) const;

  // direct memory access functions
  uint8 getByte(uint16 addr) const;
  uint16 getTwoBytes(uint16 addr) const;
  uint8 *getPtr(uint16 addr);
  uint8 &getRef(uint16 addr);
};
