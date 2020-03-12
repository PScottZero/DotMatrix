#include "cpu.h"

// Initializes cpu data
CPU::CPU() {
    A = B = C = D = E = H = L = PC = 0;
    zero = halfCarry = subtract = carry = false;
    IME = true;
    SP = 0xFFFE;
    mem = new unsigned char[0xFFFF];
    cartStart = new unsigned char[0xFF];
    classPtr = this;
}

// Loads cartidge data into memory
void CPU::loadCartridge(string dir) {
    char* buffer = new char[0x8000];
    ifstream cartridge (dir, ios::in | ios::binary);
    cartridge.read(buffer, 0x8000);
    if (buffer) {
        for (unsigned short index = 0; index < 0x8000; index++) {
            if (index < 0x100) {
                cartStart[index] = (unsigned char)buffer[index];
            }
            else {
                mem[index] = (unsigned char)buffer[index];
            }
        }
    }
    delete[] buffer;
}

// Loads gameboy bootstrap
void CPU::loadBootstrap() {
    char buffer[0x100];
    ifstream bootstrap ("C:/Users/8psco/Documents/GitHub/DotMatrix/bootstrap.bin", ios::in | ios::binary);
    bootstrap.read(buffer, 0x100);
    unsigned short index = 0;
    for (char byte : buffer) {
        mem[index] = byte;
        index++;
    }
}

// Run emulator
// TODO: fully implement run function
void CPU::step() {
    if (PC == 0x100) {
        for (int index = 0; index < 0x100; index++) {
            mem[index] = cartStart[index];
        }
    }
    decode(mem[PC]);
}

// Decode instruction
void CPU::decode(unsigned char opcode) {

    unsigned char upperTwo = opcode >> 6 & 0b11;
    unsigned char cond = (opcode >> 3) & 0b11;
    unsigned char regDest = opcode >> 3 & 0b111;
    unsigned char regSrc = opcode & 0b111;
    unsigned char lo = opcode & 0xF;
    unsigned char regPair = opcode >> 4 & 0b11;


    // ======================================================================================
    // ---------------------------- OPCODES IN NON-GENERAL FORM -----------------------------
    // ======================================================================================
    switch (opcode) {

    // LD (HL), n
    // Load 8-bit immediate n into memory at address HL
    case 0x36:
        writeMem(getRegPair(HL), getImm8());
        clock += 3;
        break;

    // LD A, (BC)
    // Load memory at address BC into register A
    case 0x0A:
        A = readMem(getRegPair(BC));
        clock += 2;
        break;

    // LD A, (DE)
    // Load memory at address DE into register A
    case 0x1A:
        A = readMem(getRegPair(DE));
        clock += 2;
        break;

    // LD A, (C)
    // Load memory at address 0xFF00 + [register C] into register A
    case 0xF2:
        A = readMem(0xFF00 + C);
        clock += 2;
        break;

    // LD (C), A
    // Load register A into memory at address 0xFF00 + [register C]
    case 0xE2:
        writeMem(0xFF00 + C, A);
        clock += 2;
        break;

    // LD A, (n)
    // Load memory at address 0xFF00 + [8-bit immediate n] into register A
    case 0xF0:
        A = readMem(0xFF00 + getImm8());
        clock += 3;
        break;

    // LD (n), A
    // Load register A into memory at address 0xFF00 + [8-bit immediate n]
    case 0xE0:
        writeMem(0xFF00 + getImm8(), A);
        clock += 3;
        break;

    // LD A, (nn)
    // Load memory at address specified by 16-bit immediate nn into register A
    case 0xFA:
        A = readMem(getImm16());
        clock += 4;
        break;

    // LD (nn), A
    // Load register A into memory at address specified by 16-bit immediate nn
    case 0xEA:
        writeMem(getImm16(), A);
        clock += 4;
        break;

    // LD A, (HLI)
    // Load memory at address HL into register A, then increment HL
    case 0x2A:
        A = readMem(getRegPair(HL));
        setRegPair(HL, getRegPair(HL) + 1);
        clock += 2;
        break;

    // LD A, (HLD)
    // Load memory at address HL into register A, then decrement HL
    case 0x3A:
        A = readMem(getRegPair(HL));
        setRegPair(HL, getRegPair(HL) - 1);
        clock += 2;
        break;

    // LD (BC), A
    // Load register A into memory at address BC
    case 0x02:
        writeMem(getRegPair(BC), A);
        clock += 2;
        break;

    // LD (DE), A
    // Load register A into memory at address DE
    case 0x12:
        writeMem(getRegPair(DE), A);
        clock += 2;
        break;

    // LD (HLI), A
    // Load register A into memory at address HL, then increment HL
    case 0x22:
        writeMem(getRegPair(HL), A);
        setRegPair(HL, getRegPair(HL) + 1);
        clock += 2;
        break;

    // LD (HLD), A
    // Load register A into memory at address HL, then decrement HL
    case 0x32:
        writeMem(getRegPair(HL), A);
        setRegPair(HL, getRegPair(HL) - 1);
        clock += 2;
        break;

    // LD SP, HL
    // Load register HL into SP
    case 0xF9:
        SP = getRegPair(HL);
        clock += 2;
        break;

    // LDHL SP, e
    // Add 8-bit immediate e to SP and store result in HL
    case 0xF8:
        setRegPair(HL, add16(SP, getImm8()));
        zero = false;
        clock += 3;
        break;

    // LD (nn), SP
    // Load stack pointer into memory at address specified by 16-bit immediate nn
    case 0x08:
        writeMem(getImm16(), SP);
        clock += 5;
        break;

    // ADD A, n
    // Add 8-bit immediate n with register A and store result in A
    case 0xC6:
        A = add(A, getImm8(), NO_CARRY);
        clock += 2;
        break;

    // ADD A, (HL)
    // Add memory at address HL to register A and store result in A
    case 0x86:
        A = add(A, readMem(getRegPair(HL)), NO_CARRY);
        clock += 2;
        break;

    // ADC A, n
    // Add A, 8-bit immediate n, and carry flag together and store result in A
    case 0xCE:
        A = add(A, getImm8(), WITH_CARRY);
        clock += 2;
        break;

    // ADC A, (HL)
    // Add A, memory at address HL, and carry flag together and store result in A
    case 0x8E:
        A = add(A, readMem(getRegPair(HL)), WITH_CARRY);
        clock += 2;
        break;

    // SUB n
    // Subtract 8-bit immediate n from register A and store result in A
    case 0xD6:
        A = sub(A, getImm8(), NO_CARRY);
        clock += 2;
        break;

    // SUB (HL)
    // Subtract memory at address HL from register A and store result in
    case 0x96:
        A = sub(A, readMem(getRegPair(HL)), NO_CARRY);
        clock += 2;
        break;

    // SBC A, n
    // Subtract 8-bit immediate n and carry flag from register A and store result in A
    case 0xDE:
        A = sub(A, getImm8(), WITH_CARRY);
        clock += 2;
        break;

    // SBC A, (HL)
    // Subtract memory at address HL and carry flag from register A and store result in A
    case 0x9E:
        A = sub(A, readMem(getRegPair(HL)), WITH_CARRY);
        clock += 2;
        break;

    // AND n
    // And register A and 8-bit immediate n together and store result in A
    case 0xE6:
        A = bitAnd(A, getImm8());
        clock += 2;
        break;

    // AND (HL)
    // And register A and memory at address HL together and store result in A
    case 0xA6:
        A = bitAnd(A, readMem(getRegPair(HL)));
        clock += 2;
        break;

    // OR n
    // Or register A and 8-bit immediate n together and store result in A
    case 0xF6:
        A = bitOr(A, getImm8());
        clock += 2;
        break;

    // OR (HL)
    // Or register A and memory at address HL together and store result in A
    case 0xB6:
        A = bitOr(A, readMem(getRegPair(HL)));
        clock += 2;
        break;

    // XOR n
    // Xor register A and 8-bit immediate n together and store result in A
    case 0xEE:
        A = bitXor(A, getImm8());
        clock += 2;
        break;

    // XOR (HL)
    // Xor register A and memory at address HL together and store result in A
    case 0xAE:
        A = bitXor(A, readMem(getRegPair(HL)));
        clock += 2;
        break;

    // CP n
    // Compare register A and 8-bit immediate n
    case 0xFE:
        sub(A, getImm8(), NO_CARRY);
        clock += 2;
        break;

    // CP (HL)
    // Compare register A and memory at address HL
    case 0xBE:
        sub(A, readMem(getRegPair(HL)), NO_CARRY);
        clock += 2;
        break;

    // INC (HL)
    // Increment memory at address HL
    case 0x34:
        writeMem(getRegPair(HL), inc(readMem(getRegPair(HL))));
        clock += 3;
        break;

    // DEC (HL)
    // Decrement memory at address HL
    case 0x35:
        writeMem(getRegPair(HL), dec(readMem(getRegPair(HL))));
        clock += 3;
        break;

    // ADD SP, e
    // Add 8-bit immediate e to SP and store result in SP
    case 0xE8:
        SP = add16(SP, getImm8());
        clock += 4;
        break;

    // RLCA
    // Rotate register A to the left
    case 0x07:
        A = rotateLeft(A);
        clock += 1;
        break;

    // RLA
    // Rotate register A to the left through the carry flag
    case 0x17:
        A = rotateLeftCarry(A);
        clock += 1;
        break;

    // RRCA
    // Rotate register A to the right
    case 0x0F:
        A = rotateRight(A);
        clock += 1;
        break;

    // RRA
    // Rotate register A to the right through the carry flag
    case 0x1F:
        A = rotateRightCarry(A);
        clock += 1;
        break;

    // JP nn
    // Jump to address specified by 16 bit immediate
    case 0xC3:
        jump(getImm16());
        clock += 4;
        break;

    // JR e
    // Jump to PC + (signed) e
    case 0x18:
        jumpRel(getImm8());
        clock += 3;
        break;

    // JP (HL)
    // Jump to address specified by register HL
    case 0xE9:
        jump(getRegPair(HL));
        clock += 1;
        break;

    // CALL nn
    // Push current address onto stack and jump to address specified by 16 bit immediate
    case 0xCD:
        call(getImm16());
        clock += 6;
        break;

    // RET
    // Pop stack and return to popped address
    case 0xC9:
        ret();
        clock += 4;
        break;

    // RETI
    // Return from interrupt routine
    case 0xD9:
        ret();
        clock += 4;
        IME = true;
        break;

    // RST t
    // Load zero page memory address specified by t into PC
    case 0xC7:
        PC = 0xFFFF;
        clock += 4;
        break;
    case 0xCF:
        PC = 0x07;
        clock += 4;
        break;
    case 0xD7:
        PC = 0x0F;
        clock += 4;
        break;
    case 0xDF:
        PC = 0x17;
        clock += 4;
        break;
    case 0xE7:
        PC = 0x1F;
        clock += 4;
        break;
    case 0xEF:
        PC = 0x27;
        clock += 4;
        break;
    case 0xF7:
        PC = 0x2F;
        clock += 4;
        break;
    case 0xFF:
        PC = 0x37;
        clock += 4;
        break;

    // DAA
    // Decimal adjust accumulator
    case 0x27:
        decimalAdjustAcc();
        clock += 1;
        break;

    // CPL
    // Complement register A
    case 0x2F:
        A = ~A;
        clock += 1;
        break;

    // NOP
    // No operation
    case 0x00:
        clock += 1;
        break;

    // CCF
    // Complement carry flag
    case 0x3F:
        carry = !carry;
        clock += 1;
        break;

    // SCF
    // Set carry flag
    case 0x37:
        carry = true;
        clock += 1;
        break;

    // DI
    // Reset interrupt master enable flag
    case 0xF3:
        IME = false;
        clock += 1;
        break;

    // EI
    // Set interrupt master enable flag
    case 0xFB:
        IME = true;
        clock += 1;
        break;

    // HALT
    // stop system clock
    // TODO: implement halt instruction
    case 0x76:
        clock += 1;
        break;

    // STOP
    // Stop system clock and oscillator circuit
    // TODO: implement stop instruction
    case 0x10:
        clock += 1;
        break;



    // ======================================================================================
    // --------------------------------- CB PREFIX OPCODES ----------------------------------
    // ======================================================================================
    case 0xCB:

        unsigned char imm8 = getImm8();
        upperTwo = imm8 >> 6 & 0b11;
        regDest = imm8 >> 3 & 0b111;
        regSrc = imm8 & 0b111;

        // ======================================================================================
        // ----------------------- CB PREFIX OPCODES IN NON-GENERAL FORM ------------------------
        // ======================================================================================
        switch (imm8) {

        // RLC (HL)
        // Rotate memory at address HL to the left
        case 0x06:
            writeMem(getRegPair(HL), rotateLeft(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // RL (HL)
        // Rotate memory at address HL to the left through carry flag
        case 0x16:
            writeMem(getRegPair(HL), rotateLeftCarry(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // RRC (HL)
        // Rotate memory at address HL to the right
        case 0x0E:
            writeMem(getRegPair(HL), rotateLeft(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // RR (HL)
        // Rotate memory at address HL to the left through carry flag
        case 0x1E:
            writeMem(getRegPair(HL), rotateRightCarry(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // SLA (HL)
        // Shift memory at register HL left
        case 0x26:
            writeMem(getRegPair(HL), shiftLeft(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // SRA (HL)
        // Shift memory at HL to the right
        case 0x2E:
            writeMem(getRegPair(HL), shiftRightArithmetic(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // SRL (HL)
        // Shift memory at HL to the right logically
        case 0x3E:
            writeMem(getRegPair(HL), shiftRightLogical(readMem(getRegPair(HL))));
            clock += 4;
            break;

        // SWAP (HL)
        // Swap upper and lower nibbles of memory at HL
        case 0x36:
            writeMem(getRegPair(HL), swap(readMem(getRegPair(HL))));
            clock += 4;
            break;
        }


        // ======================================================================================
        // ------------------------- CB PREFIX OPCODES IN GENERAL FORM --------------------------
        // ======================================================================================
        switch (upperTwo) {

        // ========================
        // OPCODES OF FORM 00XXXRRR
        // ========================
        case 0b00:

            switch (regDest) {

            // RLC r
            // Rotate register r to the left
            case 0b000:
                *(regArr[regSrc]) = rotateLeft(*(regArr[regSrc]));
                clock += 2;
                break;

            // RRC r
            // Rotate register r to the right
            case 0b001:
                *(regArr[regSrc]) = rotateRight(*(regArr[regSrc]));
                clock += 2;
                break;

            // RL r
            // Rotate register r to the left through carry flag
            case 0b010:
                *(regArr[regSrc]) = rotateLeftCarry(*(regArr[regSrc]));
                clock += 2;
                break;

            // RR r
            // Rotate register r to the right through carry flag
            case 0b011:
                *(regArr[regSrc]) = rotateRightCarry(*(regArr[regSrc]));
                clock += 2;
                break;

            // SLA r
            // Shift register r to the left
            case 0b100:
                *(regArr[regSrc]) = shiftLeft(*(regArr[regSrc]));
                clock += 2;
                break;

            // SRA r
            // Shift register r to the right
            case 0b101:
                *(regArr[regSrc]) = shiftRightArithmetic(*(regArr[regSrc]));
                clock += 2;
                break;

            // SWAP r
            // Swap upper and lower nibbles of register r
            case 0b110:
                *(regArr[regSrc]) = swap(*(regArr[regSrc]));
                clock += 2;
                break;

            // SRL r
            // Shift register r to the right logically
            case 0b111:
                *(regArr[regSrc]) = shiftRightLogical(*(regArr[regSrc]));
                clock += 2;
                break;
            }
            break;

        // ========================
        // OPCODES OF FORM 01BBBRRR
        // ========================
        case 0b01:

            // BIT b, (HL)
            // Copies complement of specified bit b of memory at HL to Z flag
            if (regSrc == 0b110) {
                compBitToZero(readMem(getRegPair(HL)), regDest);
                clock += 3;
            }

            // BIT b, r
            // Copies complement of specified bit b of register r to Z flag
            else {
                compBitToZero(*(regArr[regSrc]), regDest);
                clock += 2;
            }
            break;

        // ========================
        // OPCODES OF FORM 10BBBRRR
        // ========================
        case 0b10:

            // RES b, (HL)
            // Reset bit b in memory at HL to 0
            if (regSrc == 0b110) {
                writeMem(getRegPair(HL), resetBit(readMem(getRegPair(HL)), regDest));
                clock += 4;
            }

            // RES b, r
            // Set bit b in register r to 0
            else {
                *(regArr[regSrc]) = resetBit(*(regArr[regSrc]), regDest);
                clock += 2;
            }
            break;

        // ========================
        // OPCODES OF FORM 11BBBRRR
        // ========================
        case 0b11:

            // SET b, (HL)
            // Sets bit b in memory at HL to 1
            if (regSrc == 0b110) {
                writeMem(getRegPair(HL), setBit(readMem(getRegPair(HL)), regDest));
                clock += 4;
            }

            // SET b, r
            // Sets bit b in register r to 1
            else {
                *(regArr[regSrc]) = setBit(*(regArr[regSrc]), regDest);
                clock += 2;
            }
            break;
        }
        break;
    }



    // ======================================================================================
    // ------------------------------ OPCODES IN GENERAL FORM -------------------------------
    // ======================================================================================
    switch (upperTwo) {

    case 0b00:

        // ========================
        // OPCODES OF FORM 00RRRXXX
        // ========================
        switch (regSrc) {

        // JR cc, e
        // Jump to PC + (signed) e if condition cc is met
        case 0b000:
            condition(JUMP_REL, cond, getImm8(), 3, 2);
            break;

        // INC r
        // Increment register r
        case 0b100:
            *(regArr[regDest]) = inc(*(regArr[regDest]));
            clock += 1;
            break;

        // DEC r
        // Decrement register r
        case 0b101:
            *(regArr[regDest]) = dec(*(regArr[regDest]));
            clock += 1;
            break;

        // LD r, n
        // Load 8-bit immediate n into register r
        case 0b110:
            *regArr[regDest] = getImm8();
            clock += 2;
            break;
        }

        // ========================
        // OPCODES OF FORM 00DDXXXX
        // ========================
        switch (lo) {

        // LD dd, nn
        // Load 16-bit immediate nn into register pair dd
        case 0x1:
            setRegPair(regPair, getImm16());
            clock += 3;
            break;

        // INC ss
        // Increment register ss
        case 0x3:
            setRegPair(regPair, getRegPair(regPair) + 1);
            clock += 2;
            break;

        // ADD HL, ss
        // Add register HL and register ss and store store result in HL
        case 0x9:
            setRegPair(HL, add16(getRegPair(HL), getRegPair(regPair)));
            clock += 2;
            break;

        // DEC ss
        // Decrement register ss
        case 0xB:
            setRegPair(regPair, getRegPair(regPair) - 1);
            clock += 2;
            break;
        }
        break;

    case 0b01:

        // LD r, (HL)
        // Load memory at register pair HL into register r
        if (regSrc == 0b110) {
            *(regArr[regDest]) = readMem(getRegPair(HL));
            clock += 2;
        }

        // LD (HL), e
        // Load contents of register r into memory location
        // specified by register pair HL
        else if (regDest == 0b110) {
            writeMem(getRegPair(HL), *(regArr[regSrc]));
            clock += 2;
        }

        // LD r, r'
        // Load contents of register r' into register r
        else {
            *(regArr[regDest]) = *(regArr[regSrc]);
            clock += 1;
        }
        break;

    case 0b10:

        // ========================
        // OPCODES OF FORM 10XXXRRR
        // ========================
        switch (regDest) {

        // ADD A, r
        // Add register r to register A and store result in A
        case 0b000:
            A = add(A, *(regArr[regSrc]), NO_CARRY);
            clock += 1;
            break;

        // ADC A, r
        // Add A, register r, and carry flag together
        // and store result in A
        case 0b001:
            A = add(A, *(regArr[regSrc]), WITH_CARRY);
            clock += 1;
            break;

        // SUB r
        // Subtract register r from register A and store result in A
        case 0b010:
            A = sub(A, *(regArr[regSrc]), NO_CARRY);
            clock += 1;
            break;

        // SBC A, r
        // Subtract register r and carry flag from
        // register A and store result in A
        case 0b011:
            A = sub(A, *(regArr[regSrc]), WITH_CARRY);
            clock += 1;
            break;

        // AND r
        // And register A and register r together and store result in A
        case 0b100:
            A = bitAnd(A, *(regArr[regSrc]));
            clock += 1;
            break;

        // XOR r
        // Xor register A and register r together and store result in A
        case 0b101:
            A = bitXor(A, *(regArr[regSrc]));
            clock += 1;
            break;

        // OR r
        // Or register A and register r together and store result in A
        case 0b110:
            A = bitOr(A, *(regArr[regSrc]));
            clock += 1;
            break;

        // CP r
        // Compare register A and register R
        case 0b111:
            sub(A, *(regArr[regSrc]), NO_CARRY);
            clock += 1;
            break;
        }

    case 0b11:

        // PUSH qq
        // Push register pair qq onto stack
        if (lo == 0x5) {
            pushRegPair(regPair);
            clock += 4;
        }

        // POP qq
        // Pop contents of stack into register pair qq
        else if (lo == 0x1) {
            popRegPair(regPair);
            clock += 3;
        }

        // RET cc
        // Return from subroutine if condition cc is true
        else if (regSrc == 0b000) {
            condition(RETURN, cond, 0, 5, 2);
        }

        // JP cc, nn
        // Jump to address specific if condition cc is met
        else if (regSrc == 0b010) {
            condition(JUMP, cond, getImm16(), 4, 3);
        }

        // CALL cc, nn
        // Call address at nn if condition cc is met
        else if (regSrc == 0b100) {
            condition(CALL, cond, getImm16(), 6, 3);
        }
        break;
    }
    PC++;
}

unsigned char CPU::readMem(unsigned short addr) {
    return mem[addr];
}

void CPU::writeMem(unsigned short addr, unsigned char value) {
    mem[addr] = value;
}

// Add opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a + b (unsigned char)
unsigned char CPU::add(unsigned char a, unsigned char b, bool withCarry) {
    unsigned char c = carry;
    if (!withCarry) {
        c = 0;
    }
    unsigned short result = a + b + c;
    zero = (result & 0xFF) == 0;
    halfCarry = (a & 0xF) + (b & 0xF) > 0xF;
    subtract = false;
    carry = result > 0xFF;
    return (unsigned char)result;
}

// Add opertaion for 16-bit numbers
// Input: a (unsigned short), b (unsigned short)
// Output: a + b (unsigned short)
unsigned short CPU::add16(unsigned short a, unsigned short b) {
    unsigned int result = a + b;
    halfCarry = (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
    subtract = false;
    carry = result > 0xFFFF;
    return (unsigned short)result;
}

// Subtract opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a - b (unsigned char)
unsigned char CPU::sub(unsigned char a, unsigned char b, bool withCarry) {
    carry = !carry;
    return add(a, ~b + 1, withCarry);
}

// ANDs two 8-bit numbers together
// Input: a (unsigned char), b (unsigned char)
// Output: a & b
unsigned char CPU::bitAnd(unsigned char a, unsigned char b) {
    unsigned char result = a & b;
    zero = result == 0;
    halfCarry = true;
    subtract = false;
    carry = false;
    return result;
}

// ORs two 8-bit numbers together
// Input: a (unsigned char), b (unsigned char)
// Output: a | b
unsigned char CPU::bitOr(unsigned char a, unsigned char b) {
    unsigned char result = a | b;
    zero = result == 0;
    return result;
}

// XORs two 8-bit numbers together
// Input: a (unsigned char), b (unsigned char)
// Output: a ^ b
unsigned char CPU::bitXor(unsigned char a, unsigned char b) {
    unsigned char result = a ^ b;
    zero = result == 0;
    return result;
}

// Increments value by one
// Input: value (unsigned char)
// Output: value + 1
unsigned char CPU::inc(unsigned char value) {
    unsigned char result = value + 1;
    zero = result == 0;
    halfCarry = (value & 0xF) + 1 > 0xF;
    subtract = false;
    return result;
}

// Decrements value by one
// Input: value (unsigned char)
// Output: value - 1
unsigned char CPU::dec(unsigned char value) {
    unsigned char result = value - 1;
    zero = result == 0;
    halfCarry = (value & 0xF) - 1 < 0;
    subtract = true;
    return result;
}

// Rotates value to the left
// Input: value (unsigned char)
// Output: rotated value
unsigned char CPU::rotateLeft(unsigned char value) {
    unsigned char bit7 = (value & 0x80) >> 7;
    unsigned char result = (value << 1) | bit7;
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = bit7;
    return result;
}

// Rotates value to the left through carry flag
// Input: value (unsigned char)
// Output: rotated value
unsigned char CPU::rotateLeftCarry(unsigned char value) {
    unsigned char result = (value << 1) | (unsigned char)carry;
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = (value & 0x80) >> 7;
    return result;
}

// Rotates value to the right
// Input: value (unsigned char)
// Output: rotated value
unsigned char CPU::rotateRight(unsigned char value) {
    unsigned char result = (value >> 1) | ((value & 0x1) << 7);
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = value & 0x1;
    return result;
}

// Rotates value to the right through carry flag
// Input: value (unsigned char)
// Output: rotated value
unsigned char CPU::rotateRightCarry(unsigned char value) {
    unsigned char result = (value >> 1) | (carry << 7);
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = value & 0x1;
    return result;
}

// Shifts value to the left by one
// Input: value (unsigned char)
// Output: value << 1
unsigned char CPU::shiftLeft(unsigned char value) {
    unsigned char result = value << 1;
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = (value & 0x80) >> 7;
    return result;
}

// Arithmetically shifts value to the right by one
// Input: value (unsigned char)
// Output: value >> 1
unsigned char CPU::shiftRightArithmetic(unsigned char value) {
    unsigned char msb = value & 0x80;
    unsigned char result = (value >> 1) | msb;
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = value & 0x1;
    return result;
}

// Logically shifts value to the right by one
// Input: value (unsigned char)
// Output: value >> 1
unsigned char CPU::shiftRightLogical(unsigned char value) {
    unsigned char result = (value >> 1) & 0x7F;
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = value & 0x1;
    return result;
}

// Swaps lower and upper nibbles of specified value
// Input: value (unsigned char)
// Output: swapped value (unsigned char)
unsigned char CPU::swap(unsigned char value) {
    unsigned char result = ((value << 4) & 0xF0) | ((value >> 4) & 0xF);
    zero = (result == 0);
    halfCarry = false;
    subtract = false;
    carry = false;
    return result;
}

// Copies complement of bit in value into zero flag
// Input: value (unsigned char), bit (int)
// Output: none
void CPU::compBitToZero(unsigned char value, unsigned char bit) {
    zero = !(bool)((value >> bit) & 0x1);
    halfCarry = true;
    subtract = false;
}

// Sets specified bit to 1
// Input: value (unsigned char), bit (int)
// Output: value with specified bit set to 1 (unsigned char)
unsigned char CPU::setBit(unsigned char value, unsigned int bit) {
    return value | (1 << bit);
}

// Sets specified bit to 0
// Input: value (unsigned char), bit (int)
// Output: value with specified bit set to 0 (unsigned char)
unsigned char CPU::resetBit(unsigned char value, unsigned int bit) {
    return value & ~(1 << bit);
}

void CPU::decimalAdjustAcc() {
    if (!subtract) {
        if (halfCarry || (A & 0xF) > 0x9) {
            A += 0x6;
        }
        if (carry || A > 0x99) {
            A += 0x60;
            carry = true;
        }
    } else {
        if (carry) {
            A -= 0x60;
        }
        if (halfCarry) {
            A -= 0x6;
        }
    }
    zero = A == 0;
    halfCarry = 0;
}

void CPU::pushRegPair(unsigned char regPair) {
    switch (regPair) {
    case BC:
        writeMem(SP--, B);
        writeMem(SP--, C);
        break;
    case DE:
        writeMem(SP--, D);
        writeMem(SP--, E);
        break;
    case HL:
        writeMem(SP--, H);
        writeMem(SP--, L);
        break;
    case AF_SP:
        writeMem(SP--, A);
        writeMem(SP--, getF());
        break;
    }
}

void CPU::popRegPair(unsigned char regPair) {
    switch (regPair) {
    case BC:
        C = readMem(++SP);
        B = readMem(++SP);
        break;
    case DE:
        D = readMem(++SP);
        E = readMem(++SP);
        break;
    case HL:
        H = readMem(++SP);
        L = readMem(++SP);
        break;
    case AF_SP:
        A = readMem(++SP);
        setF(readMem(++SP));
        break;
    }
}

unsigned char CPU::getF() {
    return 0x00 | zero << 7 | subtract << 6 | halfCarry << 5 | carry << 4;
}

unsigned char CPU::getImm8() {
    return readMem(++PC);
}

unsigned short CPU::getImm16() {
    unsigned short imm16 = readMem(++PC);
    return imm16 | (readMem(++PC) << 8);
}

void CPU::setF(unsigned char value) {
    zero = value >> 7 & 0x1;
    subtract = value >> 6 & 0x1;
    halfCarry = value >> 5 & 0x1;
    carry = value >> 4 & 0x1;
}

void CPU::setRegPair(unsigned char regPair, unsigned short value) {
    switch (regPair) {
    case BC:
        B = value >> 8;
        C = (unsigned char)value;
        break;
    case DE:
        D = value >> 8;
        E = (unsigned char)value;
        break;
    case HL:
        H = value >> 8;
        L = (unsigned char)value;
        break;
    case AF_SP:
        SP = value;
        break;
    }
}

void CPU::jump(unsigned short imm) {
    PC = imm - 1;
}

void CPU::jumpRel(char imm) {
    PC += (char)imm;
}

void CPU::call(unsigned short imm) {
    writeMem(SP--, (PC >> 8) & 0xFF);
    writeMem(SP--, PC & 0xFF);
    PC = imm - 1;
}

void CPU::ret() {
    unsigned char lo = readMem(++SP);
    PC = lo | (readMem(++SP) << 8) + 2;
}

void CPU::condition(Control condFunc, unsigned char condValue,
                    unsigned short imm, int clockSuccess, int clockFail) {
    bool cond;
    switch (condValue) {
    case NOT_ZERO:
        cond = !zero;
        break;
    case ZERO:
        cond = zero;
        break;
    case NOT_CARRY:
        cond = !carry;
        break;
    case CARRY:
        cond = carry;
        break;
    }

    if (cond) {
        switch (condFunc) {
        case JUMP:
            jump(imm);
            break;
        case JUMP_REL:
            jumpRel(imm);
            break;
        case CALL:
            call(imm);
            break;
        case RETURN:
            ret();
            break;
        }
        clock += clockSuccess;
    } else {
        clock += clockFail;
    }
}
