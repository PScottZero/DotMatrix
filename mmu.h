#ifndef DOTMATRIX_MMU_H
#define DOTMATRIX_MMU_H

#include <iostream>
#include <fstream>
#include <unordered_map>

constexpr auto BANK_SIZE = 0x4000;

// ================================
// Register memory addresses
// ================================
constexpr auto BANK_TYPE = 0x147;
constexpr auto WORK_RAM = 0xC000;
constexpr auto ECHO_START = 0xE000;
constexpr auto ECHO_END = 0xFE00;
constexpr auto JOYPAD = 0xFF00;
constexpr auto DIVIDER = 0xFF04;
constexpr auto COUNTER = 0xFF05;
constexpr auto MODULO = 0xFF06;
constexpr auto TIMER_CONTROL = 0xFF07;
constexpr auto DMA = 0xFF46;

// ================================
// OAM constants
// ================================
constexpr auto OAM_ADDR = 0xFE00;
constexpr auto OAM_COUNT = 40;
constexpr auto BYTES_PER_OAM = 4;

// ================================
// ROM banking types
// ================================
enum BankType {
    NONE = 0x00,
    MBC1 = 0x01, MBC1_RAM = 0x02, MBC1_BAT_RAM = 0x03,
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
    std::unordered_map<Joypad, bool> joypad;

    MMU();
    [[nodiscard]] unsigned char read(unsigned short) const;
    void write(unsigned short, unsigned char);
    void loadCartridge(const std::string&) const;
    void loadBank(unsigned char) const;
    void checkForInput();
};


#endif //DOTMATRIX_MMU_H
