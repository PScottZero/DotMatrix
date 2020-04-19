#ifndef CPU_H
#define CPU_H

#include "mmu.h"
#include "ppu.h"

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
    unsigned int clock;
    unsigned int clockPrev;
    unsigned int internalDivider;
    unsigned int internalCounter;
    bool halted;
    MMU *mmu;

    // ================================
    // Emulator functions
    // ================================
    CPU();
    void decode(unsigned char opcode);
    void step();
    void incTimers();
    void reset();

    // ================================
    // Memory access functions
    // ================================
    void pushRegPair(unsigned char regPair);
    void popRegPair(unsigned char regPair);
    void push(unsigned short value);
    unsigned short pop();
    [[nodiscard]] unsigned char getF() const;
    [[nodiscard]] unsigned short getRegPair(unsigned char regPair) const;
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
    bool vblankIntTriggered() const;
    bool lcdIntTriggered() const;
    bool timerIntTriggered() const;
    bool serialIntTriggered() const;
    bool joypadIntTriggered() const;
};

#endif // CPU_H
