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
