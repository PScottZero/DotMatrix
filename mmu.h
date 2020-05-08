#ifndef DOTMATRIX_MMU_H
#define DOTMATRIX_MMU_H

#include <iostream>
#include <fstream>
#include <unordered_map>

constexpr auto BANK_SIZE = 0x4000;

// ================================
// Memory map addresses
// ================================
constexpr auto ROM_BANK_0_START = 0x0000;
constexpr auto ROM_BANK_0_END = 0x4000;
constexpr auto ROM_BANK_1_START = 0x4000;
constexpr auto ROM_BANK_1_END = 0x8000;
constexpr auto VRAM_START = 0x8000;
constexpr auto VRAM_END = 0xA000;
constexpr auto EXT_RAM_START = 0xA000;
constexpr auto EXT_RAM_END = 0xC000;
constexpr auto WORK_RAM_START = 0xC000;
constexpr auto WORK_RAM_ECHO_END = 0xDE00;
constexpr auto ECHO_OFFSET = 0x2000;

// ================================
// Register memory addresses
// ================================
constexpr auto BANK_TYPE = 0x147;
constexpr auto JOYPAD = 0xFF00;
constexpr auto DIVIDER = 0xFF04;
constexpr auto TIMA = 0xFF05;
constexpr auto TMA = 0xFF06;
constexpr auto TAC = 0xFF07;
constexpr auto DMA = 0xFF46;

// ================================
// OAM constants
// ================================
constexpr auto OAM_ADDR = 0xFE00;
constexpr auto OAM_COUNT = 40;
constexpr auto BYTES_PER_OAM = 4;

// ================================
// Banking Modes (MBC1)
// ================================
constexpr auto ROM_BANKING_MODE = 0;
constexpr auto RAM_BANKING_MODE = 1;

// ================================
// Memory Sizes
// ================================
constexpr auto GB_MEM_SIZE = 0x10000;
constexpr auto EXT_RAM_SIZE = 0x10000;
constexpr auto CART_SIZE = 0x200000;

// ================================
// ROM banking types
// ================================
enum BankType {
    NONE, MBC1, MBC2, MBC3, MBC5
};

// ================================
// Joypad controls
// ================================
enum Joypad {
    RIGHT, LEFT, UP, DOWN,
    START, SELECT, BUTTON_A, BUTTON_B
};

class MMU {
public:
    unsigned char* mem;
    char* cart;
    char* ram;
    std::string cartDir;
    bool hasRAM;
    bool ramEnabled;
    bool bankMode;
    unsigned int bankUpperBits;
    unsigned int bankLowerBits;
    unsigned int ramBank;
    BankType bankType;
    std::unordered_map<Joypad, bool> joypad;

    MMU();
    [[nodiscard]] unsigned char read(unsigned short) const;
    void write(unsigned short, unsigned char);
    void loadCartridge(std::string);
    void loadRAM() const;
    void saveRAM() const;
    void checkForInput();
    void checkForMBCRequest(unsigned short addr, unsigned char value);
    void checkForDMATransfer(unsigned short addr, unsigned char value);
    void checkForEcho(unsigned short addr, unsigned char value) const;
};

#endif //DOTMATRIX_MMU_H
