#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

constexpr auto BANK_SIZE = 0x4000;

// ================================
// Registers
// ================================
constexpr auto BC = 0b00;
constexpr auto DE = 0b01;
constexpr auto HL = 0b10;
constexpr auto AF_SP = 0b11;
constexpr auto MEM = 0b110;
constexpr auto IE = 0xFFFF;
constexpr auto IF = 0xFF0F;

// ================================
// Conditions
// ================================
constexpr auto NOT_ZERO = 0b00;
constexpr auto ZERO = 0b01;
constexpr auto NOT_CARRY = 0b10;
constexpr auto CARRY = 0b11;

// ================================
// Carry options
// ================================
constexpr auto WITH_CARRY = true;
constexpr auto NO_CARRY = false;

// ================================
// Special memory addresses
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
// Timer constants
// ================================
constexpr auto DIVIDER_CYCLES = 64;
constexpr auto COUNTER_CYCLES_0 = DIVIDER_CYCLES * 4;
constexpr auto COUNTER_CYCLES_1 = DIVIDER_CYCLES / 16;
constexpr auto COUNTER_CYCLES_2 = DIVIDER_CYCLES / 4;
constexpr auto COUNTER_CYCLES_3 = DIVIDER_CYCLES;

// ================================
// Control flow options
// ================================
enum Control {
    JUMP, JUMP_REL, CALL, RETURN
};

enum Joypad {
    RIGHT, LEFT, UP, DOWN,
    START, SELECT, BUTTON_A, BUTTON_B
};

enum BankType {
    NONE = 0x00,
    MBC1 = 0x01, MBC1_RAM = 0x02, MBC1_BAT_RAM = 0x03,
};

class CPU
{
public:

    // ================================
    // Instance data
    // ================================
    unsigned char A, B, C, D, E, H, L;
    unsigned char* regArr[8] = { &B, &C, &D, &E, &H, &L, nullptr, &A };
    unsigned short PC, SP;
    bool zero, halfCarry, subtract, carry, IME;
    unsigned char* mem;
    char* cart;
    unsigned int clock;
    unsigned int clockPrev;
    unsigned int divider;
    unsigned int counter;
    bool halted;
    unsigned char bankType;
    unordered_map<Joypad, bool> joypad;

    // ================================
    // Emulator functions
    // ================================
    CPU();
    void decode(unsigned char opcode);
    void loadBootstrap();
    void loadCartridge(const string& dir);
    void loadBank(unsigned short bankNo);
    void step();
    void checkForInput();
    void incTimers();

    // ================================
    // Memory access functions
    // ================================
    unsigned char readMem(unsigned short addr);
    void writeMem(unsigned short addr, unsigned char value);
    void pushRegPair(unsigned char regPair);
    void popRegPair(unsigned char regPair);
    void push(unsigned short value);
    unsigned short pop();
    unsigned char getF();
    unsigned short getRegPair(unsigned char regPair);
    unsigned char getImm8();
    unsigned short getImm16();
    void setF(unsigned char value);
    void setRegPair(unsigned char regPair, unsigned short value);
    void setFlags(bool zeroCond, bool halfCond, bool subCond, bool carryCond);
    void saveSP();

    // ================================
    // Arithmetic functions
    // ================================
    unsigned char add(unsigned char a, unsigned char b, bool withCarry);
    unsigned short add16(unsigned short a, unsigned short b);
    unsigned short addSPImm8(char imm8);
    unsigned char sub(unsigned char a, unsigned char b, bool withCarry);
    unsigned char inc(unsigned char value);
    unsigned char dec(unsigned char value);
    void decimalAdjustAcc();

    // ================================
    // Logical functions
    // ================================
    unsigned char bitAnd(unsigned char a, unsigned char b);
    unsigned char bitOr(unsigned char a, unsigned char b);
    unsigned char bitXor(unsigned char a, unsigned char b);
    void compBitToZero(unsigned char value, unsigned char bit);
    static unsigned char resetBit(unsigned char value, unsigned int bit);
    unsigned char rotateLeft(unsigned char value);
    unsigned char rotateLeftCarry(unsigned char value);
    unsigned char rotateRight(unsigned char value);
    unsigned char rotateRightCarry(unsigned char value);
    unsigned char shiftLeft(unsigned char value);
    unsigned char shiftRightArithmetic(unsigned char value);
    unsigned char shiftRightLogical(unsigned char value);
    unsigned char swap(unsigned char value);
    static unsigned char setBit(unsigned char value, unsigned int bit);

    // ================================
    // Control flow functions
    // ================================
    void jump(unsigned short imm);
    void jumpRel(char imm);
    void call(unsigned short imm);
    void ret();
    void condition(Control condFunc, unsigned char condValue,
                   unsigned short imm, int clockSuccess, int clockFail);

    // ================================
    // Interrupt functions
    // ================================
    void checkForInt();
    bool vblankIntTriggered();
    bool lcdIntTriggered();
    bool timerIntTriggered();
    bool serialIntTriggered();
    bool joypadIntTriggered();
};

#endif // CPU_H
