// **************************************************
// **************************************************
// **************************************************
// MEMORY
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QString>
#include <fstream>
#include <map>
#include <vector>

#include "types.h"

// memory sizes
#define MEM_BYTES 0x8000
#define CART_BYTES 0x800000
#define ROM_BANK_BYTES 0x4000
#define RAM_BANK_BYTES 0x2000
#define HALF_RAM_BYTES 0x200
#define MAX_RAM_BANKS 16

// cartridge header addresses
#define CGB_MODE 0x0143
#define SGB_MODE 0x0146
#define BANK_TYPE 0x0147
#define ROM_SIZE 0x0148
#define RAM_SIZE 0x0149

// hardware registers
#define P1 0xFF00         // joypad register
#define SB 0xFF01         // serial transfer data
#define SC 0xFF02         // serial transfer control
#define DIV 0xFF04        // divider register
#define TIMA 0xFF05       // timer counter
#define TMA 0xFF06        // timer modulo
#define TAC 0xFF07        // timer control
#define IF 0xFF0F         // interrupt flag
#define NR10 0xFF10       // sound channel 1 sweep
#define NR11 0xFF11       // sound channel 1 length timer & duty cycle
#define NR12 0xFF12       // sound channel 1 volume & envelope
#define NR13 0xFF13       // sound channel 1 wavelength low
#define NR14 0xFF14       // sound channel 1 wavelength high & control
#define NR21 0xFF16       // sound channel 2 length timer & duty cycle
#define NR22 0xFF17       // sound channel 2 volume & envelope
#define NR23 0xFF18       // sound channel 2 wavelength low
#define NR24 0xFF19       // sound channel 2 wavelength high & control
#define NR30 0xFF1A       // sound channel 3 dac enable
#define NR31 0xFF1B       // sound channel 3 length timer
#define NR32 0xFF1C       // sound channel 3 output level
#define NR33 0xFF1D       // sound channel 3 wavelength low
#define NR34 0xFF1E       // sound channel 3 wavelength high & control
#define NR41 0xFF20       // sound channel 4 length timer
#define NR42 0xFF21       // sound channel 4 volume & envelope
#define NR43 0xFF22       // sound channel 4 frequency & randomness
#define NR44 0xFF23       // sound channel 4 control
#define NR50 0xFF24       // master volume & vin panning
#define NR51 0xFF25       // sound panning
#define NR52 0xFF26       // sound on/off
#define WAVE_RAM 0xFF30   // waveform data (up to 0xFF3F)
#define LCDC 0xFF40       // lcd control
#define STAT 0xFF41       // lcd status
#define SCY 0xFF42        // scroll y
#define SCX 0xFF43        // scroll x
#define LY 0xFF44         // lcd y coordinate
#define LYC 0xFF45        // lcd y compare
#define DMA 0xFF46        // oam dma source address & start
#define BGP 0xFF47        // background palette data (dmg only)
#define OBP0 0xFF48       // object palette 0 data (dmg only)
#define OBP1 0xFF49       // object palette 1 data (dmg only)
#define BOOTSTRAP 0xFF50  // enable/disable bootstrap
#define WY 0xFF4A         // window y position
#define WX 0xFF4B         // window x position
#define KEY1 0xFF4D       // prepare speed switch (cgb only)
#define VBK 0xFF4F        // vram bank (cgb only)
#define HDMA1 0xFF51      // vram dma source high (cgb only)
#define HDMA2 0xFF52      // vram dma source low (cgb only)
#define HDMA3 0xFF53      // vram dma destination high (cgb only)
#define HDMA4 0xFF54      // vram dma destination low (cgb only)
#define HDMA5 0xFF55      // vram dma length/mode/start (cgb only)
#define RP 0xFF56         // infrared communications port
#define BCPS 0xFF68       // background color palette specification
#define BCPD 0xFF69       // background color palette data
#define OCPS 0xFF6A       // object color palette specification
#define OCPD 0xFF6B       // object color palette data
#define OPRI 0xFF6C       // object priority mode
#define SVBK 0xFF70       // wram bank
#define PCM12 0xFF76      // audio digital outputs 1 & 2
#define PCM34 0xFF77      // audio digital outputs 3 & 4
#define IE 0xFFFF         // interrupt enable

// memory map constants
#define ROM_BANK_0_ADDR 0x0000
#define ROM_BANK_1_ADDR 0x4000
#define VRAM_ADDR 0x8000
#define EXT_RAM_ADDR 0xA000
#define WORK_RAM_ADDR 0xC000
#define RAM_ECHO_END_ADDR 0xDE00
#define ECHO_RAM_ADDR 0xE000
#define OAM_ADDR 0xFE00
#define OAM_END_ADDR 0xFEA0
#define ZERO_PAGE_ADDR 0xFF00
#define HRAM_ADDR 0xFF80

// vram + oam constants
#define _OAM_ENTRY_COUNT 40
#define _OAM_ENTRY_BYTES 4
#define _OAM_SEARCH_MODE 0b10
#define _PIXEL_TRANSFER_MODE 0b11

using namespace std;

class Memory {
 private:
  // dma tranfser
  static void dmaTransfer();

  // echo functions
  static void echoRam(uint16 addr, uint8 val);
  static void echoHalfRam(uint16 addr, uint8 val);

 public:
  // allocated memory
  static uint8 *mem;
  static uint8 *cart;
  static uint8 *exram;

  // memory banks
  static uint8 *romBank0;
  static uint8 *romBank1;
  static uint8 *exramBank;

  // memory read + write functions
  static uint8 read(uint16 addr);
  static uint16 read16(uint16 addr);
  static uint8 readBits(uint16 addr, vector<uint8> bits);
  static void write(uint16 addr, uint8 val);
  static void write(uint16 addr, uint16 val);
  static void writeBits(uint16 addr, uint16 val, vector<uint8> bits);
  static uint8 imm8(uint16 &PC);
  static uint16 imm16(uint16 &PC);
  static uint8 &getByte(uint16 addr);

  // rom + ram bank functions
  static void setRomBank(uint8 **romBank, uint8 bankNum);
  static void setExramBank(uint8 bankNum);

  // save + load functions
  static void loadRom(QString dir);
  static void loadExram();
  static void saveExram();

  // reset memory
  static void reset();
};
