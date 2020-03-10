#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <fstream>
#include <array>
#include <mutex>

using namespace std;

class CPU
{
public:
    unsigned char A, B, C, D, E, H, L;
    unsigned char* regArr[8] = { &B, &C, &D, &E, &H, &L, NULL, &A };
    unsigned short PC, SP;
    bool zero, halfCarry, subtract, carry, IME;
    unsigned char* mem;
    unsigned char* cartStart;
    mutex rw;

    CPU();

    // emulator functions
    void decode(unsigned char opcode);
    void loadBootstrap();
    void loadCartridge(string dir);
    void step();

    // memory access
    unsigned char readMem(unsigned short addr);
    void writeMem(unsigned short addr, unsigned char value);

    // opcode functions
    unsigned char add(unsigned char a, unsigned char b);
    unsigned char addCarry(unsigned char a, unsigned char b);
    unsigned short add16(unsigned short a, unsigned short b);
    unsigned char bitAnd(unsigned char a, unsigned char b);
    unsigned char bitOr(unsigned char a, unsigned char b);
    unsigned char bitXor(unsigned char a, unsigned char b);
    void compBitToZero(unsigned char value, unsigned int bit);
    unsigned char dec(unsigned char value);
    void decimalAdjustAcc();
    unsigned char inc(unsigned char value);
    unsigned char resetBit(unsigned char value, unsigned int bit);
    unsigned char rotateLeft(unsigned char value);
    unsigned char rotateLeftCarry(unsigned char value);
    unsigned char rotateRight(unsigned char value);
    unsigned char rotateRightCarry(unsigned char value);
    unsigned char shiftLeft(unsigned char value);
    unsigned char shiftRightArithmetic(unsigned char value);
    unsigned char shiftRightLogical(unsigned char value);
    unsigned char sub(unsigned char a, unsigned char b);
    unsigned char subCarry(unsigned char a, unsigned char b);
    unsigned char swap(unsigned char value);
    unsigned char setBit(unsigned char value, unsigned int bit);

    // register functions
    unsigned char getF();
    unsigned short getBC();
    unsigned short getDE();
    unsigned short getHL();
    unsigned char getImm8();
    unsigned short getImm16();
    void setF(unsigned char value);
    void setBC(unsigned short value);
    void setDE(unsigned short value);
    void setHL(unsigned short value);
};

#endif // CPU_H
