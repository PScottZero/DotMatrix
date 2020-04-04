#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <fstream>
#include <array>

using namespace std;

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
constexpr auto LCD_STAT = 0xFF41;
constexpr auto SCROLL_Y = 0xFF42;
constexpr auto SCROLL_X = 0xFF43;
constexpr auto LCDC_Y = 0xFF44;
constexpr auto LY_COMP = 0xFF45;
constexpr auto DMA = 0xFF46;
constexpr auto WINDOW_Y = 0xFF4A;
constexpr auto WINDOW_X = 0xFF4B;

// ================================
// OAM constants
// ================================
constexpr auto OAM_ADDR = 0xFE00;
constexpr auto OAM_COUNT = 40;
constexpr auto BYTES_PER_OAM = 4;

// ================================
// Control flow options
// ================================
enum Control {
    JUMP,
    JUMP_REL,
    CALL,
    RETURN
};

class CPU
{
public:

    // ================================
    // Instance data
    // ================================
    unsigned char A, B, C, D, E, H, L;
    unsigned char* regArr[8] = { &B, &C, &D, &E, &H, &L, NULL, &A };
    unsigned short PC, SP;
    bool zero, halfCarry, subtract, carry, IME;
    unsigned char* mem;
    unsigned char* cartStart;
    unsigned int clock;
    bool halted;
    unsigned char* instr;

    // ================================
    // Emulator functions
    // ================================
    CPU();
    void decode(unsigned char opcode);
    void loadBootstrap();
    void loadCartridge(string dir);
    void step();

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
    void dmaTransfer(unsigned short addr);

    // ================================
    // Arithmetic functions
    // ================================
    unsigned char add(unsigned char a, unsigned char b, bool withCarry);
    unsigned short add16(unsigned short a, unsigned short b);
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
    unsigned char resetBit(unsigned char value, unsigned int bit);
    unsigned char rotateLeft(unsigned char value);
    unsigned char rotateLeftCarry(unsigned char value);
    unsigned char rotateRight(unsigned char value);
    unsigned char rotateRightCarry(unsigned char value);
    unsigned char shiftLeft(unsigned char value);
    unsigned char shiftRightArithmetic(unsigned char value);
    unsigned char shiftRightLogical(unsigned char value);
    unsigned char swap(unsigned char value);
    unsigned char setBit(unsigned char value, unsigned int bit); 

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
