#pragma once

// **************************************************
// **************************************************
// Imports
// **************************************************
// **************************************************

#include <math.h>
#include <stdio.h>

#include <QActionGroup>
#include <QFileDialog>
#include <QImage>
#include <QMainWindow>
#include <QObject>
#include <QPixmap>
#include <QSignalMapper>
#include <QSizePolicy>
#include <QThread>
#include <array>
#include <chrono>
#include <string>
#include <thread>

using namespace std;

// **************************************************
// **************************************************
// Constants
// **************************************************
// **************************************************

// ==================================================
// CPU Constants
// ==================================================

// clock speed constants
#define DMG_CLOCK_SPEED 0x100000
#define CGB_CLOCK_SPEED 0x200000
#define CLOCK_SPEED_00_MULT 0x100
#define CLOCK_SPEED_01_MULT 0x004
#define CLOCK_SPEED_10_MULT 0x010
#define CLOCK_SPEED_11_MULT 0x040

// register constants
#define NUM_REG_16 4
#define NUM_REG_8 8
#define MEM_HL 0b110

// jump condition constants
#define JUMP_NZ 0b00
#define JUMP_Z 0b01
#define JUMP_NC 0b10
#define JUMP_C 0b11

// interrupt constants
#define JOYPAD_INT 0x10
#define SERIAL_INT 0x08
#define TIMER_INT 0x04
#define LCDC_INT 0x02
#define V_BLANK_INT 0x01

// ==================================================
// PPU Constants
// ==================================================

// clock speed constants
#define PPU_CLOCK_SPEED 0x100000
#define OAM_SEARCH_CYCLES 20
#define PIXEL_TRANSFER_CYCLES 43
#define H_BLANK_CYCLES 51
#define V_BLANK_CYCLES 114

// px constants
#define BG_PX_DIM 256
#define BG_TILE_DIM 32
#define TILE_PX_DIM 8
#define SCREEN_PX_WIDTH 160
#define SCREEN_PX_HEIGHT 144
#define SCREEN_LINES 154
#define SPRITE_PX_HEIGHT_SHORT 8
#define SPRITE_PX_HEIGHT_TALL 16

// byte constants
#define VIEW_BUF_BYTES 160 * 144
#define TILE_BYTES 16
#define OAM_ENTRY_BYTES 4

// count constants
#define BG_TILE_COUNT 32 * 32
#define OAM_ENTRY_COUNT 40
#define MAX_SPRITES_PER_LINE 10

// screen modes
#define H_BLANK_MODE 0b00
#define V_BLANK_MODE 0b01
#define OAM_SEARCH_MODE 0b10
#define PIXEL_TRANSFER_MODE 0b11

// screen memory address constants
#define WINDOW_CODE_ADDR_0 0x9800
#define WINDOW_CODE_ADDR_1 0x9C00
#define BG_DATA_ADDR_0 0x9000
#define BG_DATA_ADDR_1 0x8000
#define BG_CODE_ADDR_0 0x9800
#define BG_CODE_ADDR_1 0x9C00

// ==================================================
// Memory Constants
// ==================================================

// memory size constants
#define MEM_BYTES 0x8000
#define CART_BYTES 0x800000
#define ROM_BANK_BYTES 0x4000

// dmg in-memory flag address constants
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

// cgb in-memory flag address constants
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

// memory map constants
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
#define HRAM_ADDR 0xFF80

// ==================================================
// Miscellaneous Constants
// ==================================================

#define NS_PER_SEC 1000000000
#define PALETTE_COLOR_COUNT 4
#define WINDOW_BASE_WIDTH 640
#define WINDOW_BASE_HEIGHT 576

// **************************************************
// **************************************************
// Types
// **************************************************
// **************************************************

// integer types
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned int uint32;

// sprite oam entry
typedef struct {
  uint8 y;
  uint8 x;
  uint8 pattern;
  bool priority;
  bool flipY;
  bool flipX;
  bool palette;
} sprite_t;

// tile row of eight pixels
using TileRow = std::array<uint8, TILE_PX_DIM>;

// palette class
class Palette : public QObject {
  Q_OBJECT

 public:
  uint32 data[PALETTE_COLOR_COUNT];

  Palette(uint32 color00, uint32 color01, uint32 color10, uint32 color11)
      : data{color00, color01, color10, color11} {}
};
