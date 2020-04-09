#include "cpu.h"

// ================================
// Initialize cpu data
// ================================
CPU::CPU() {
    A = B = C = D = E = H = L = PC = 0;
    SP = 0xFFFE;
    zero = halfCarry = subtract = carry = false;
    IME = false;
    halted = false;
    mem = new unsigned char[0x10000];
    cartStart = new unsigned char[0x100];
    clock = 0;
    instr = new unsigned char[0x100];
    for (int i = 0; i < 0x100; i++) {
        instr[i] = 0;
    }
}

// ================================
// Load cartridge data into memory
// ================================
void CPU::loadCartridge(const string& dir) {
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

// ================================
// Load Game Boy bootstrap
// ================================
void CPU::loadBootstrap() {
    char buffer[0x100];
    ifstream bootstrap ("D:/Roms/GB/bootstrap.bin", ios::in | ios::binary);
//    ifstream bootstrap ("/Users/pscott/Documents/GB/bootstrap.bin", ios::in | ios::binary);
    bootstrap.read(buffer, 0x100);
    unsigned short index = 0;
    for (char byte : buffer) {
        mem[index] = byte;
        index++;
    }
}

// ================================
// Run emulator
// ================================
void CPU::step() {
    if (PC == 0x100) {
        for (int index = 0; index < 0x100; index++) {
            mem[index] = cartStart[index];
        }
    }
    checkForInt();
    if (!halted) {
        decode(mem[PC]);
    }
}

// ================================
// Decode instruction
// ================================
void CPU::decode(unsigned char opcode) {

    unsigned char upperTwo = opcode >> 6 & 0b11;
    unsigned char bitFive = opcode >> 5 & 1;
    unsigned char cond = (opcode >> 3) & 0b11;
    unsigned char regDest = opcode >> 3 & 0b111;
    unsigned char regSrc = opcode & 0b111;
    unsigned char lo = opcode & 0xF;
    unsigned char regPair = opcode >> 4 & 0b11;

    // ======================================================================================
    // ---------------------------- OPCODES IN NON-GENERAL FORM -----------------------------
    // ======================================================================================
    switch (opcode) {

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
        setRegPair(HL, add16(SP, (char)getImm8()));
        zero = false;
        clock += 3;
        break;

    // LD (nn), SP
    // Load stack pointer into memory at address specified by 16-bit immediate nn
    case 0x08:
        saveSP();
        clock += 5;
        break;

    // ADD A, n
    // Add 8-bit immediate n with register A and store result in A
    case 0xC6:
        A = add(A, getImm8(), NO_CARRY);
        clock += 2;
        break;

    // ADC A, n
    // Add A, 8-bit immediate n, and carry flag together and store result in A
    case 0xCE:
        A = add(A, getImm8(), WITH_CARRY);
        clock += 2;
        break;

    // SUB n
    // Subtract 8-bit immediate n from register A and store result in A
    case 0xD6:
        A = sub(A, getImm8(), NO_CARRY);
        clock += 2;
        break;

    // SBC A, n
    // Subtract 8-bit immediate n and carry flag from register A and store result in A
    case 0xDE:
        A = sub(A, getImm8(), WITH_CARRY);
        clock += 2;
        break;

    // AND n
    // And register A and 8-bit immediate n together and store result in A
    case 0xE6:
        A = bitAnd(A, getImm8());
        clock += 2;
        break;

    // OR n
    // Or register A and 8-bit immediate n together and store result in A
    case 0xF6:
        A = bitOr(A, getImm8());
        clock += 2;
        break;

    // XOR n
    // Xor register A and 8-bit immediate n together and store result in A
    case 0xEE:
        A = bitXor(A, getImm8());
        clock += 2;
        break;

    // CP n
    // Compare register A and 8-bit immediate n
    case 0xFE:
        sub(A, getImm8(), NO_CARRY);
        clock += 2;
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
        call(0x00);
        clock += 4;
        break;
    case 0xCF:
        call(0x08);
        clock += 4;
        break;
    case 0xD7:
        call(0x10);
        clock += 4;
        break;
    case 0xDF:
        call(0x18);
        clock += 4;
        break;
    case 0xE7:
        call(0x20);
        clock += 4;
        break;
    case 0xEF:
        call(0x28);
        clock += 4;
        break;
    case 0xF7:
        call(0x30);
        clock += 4;
        break;
    case 0xFF:
        call(0x38);
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
        unsigned char upperTwoCB = imm8 >> 6 & 0b11;
        unsigned char regDestCB = imm8 >> 3 & 0b111;
        unsigned char regSrcCB = imm8 & 0b111;

        switch (upperTwoCB) {

        // ========================
        // OPCODES OF FORM 00XXXRRR
        // ========================
        case 0b00:

            switch (regDestCB) {

            // RLC r
            // Rotate register r to the left
            case 0b000:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), rotateLeft(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = rotateLeft(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // RRC r
            // Rotate register r to the right
            case 0b001:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), rotateLeft(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = rotateRight(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // RL r
            // Rotate register r to the left through carry flag
            case 0b010:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), rotateLeftCarry(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = rotateLeftCarry(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // RR r
            // Rotate register r to the right through carry flag
            case 0b011:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), rotateRightCarry(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = rotateRightCarry(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // SLA r
            // Shift register r to the left
            case 0b100:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), shiftLeft(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = shiftLeft(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // SRA r
            // Shift register r to the right
            case 0b101:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), shiftRightArithmetic(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = shiftRightArithmetic(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // SWAP r
            // Swap upper and lower nibbles of register r
            case 0b110:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), swap(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = swap(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;

            // SRL r
            // Shift register r to the right logically
            case 0b111:
                if (regSrcCB == MEM) {
                    writeMem(getRegPair(HL), shiftRightLogical(readMem(getRegPair(HL))));
                    clock += 4;
                } else {
                    *(regArr[regSrcCB]) = shiftRightLogical(*(regArr[regSrcCB]));
                    clock += 2;
                }
                break;
            }
            break;

        // ========================
        // OPCODES OF FORM 01BBBRRR
        // ========================
        case 0b01:

            // BIT b, (HL)
            // Copies complement of specified bit b of memory at HL to Z flag
            if (regSrcCB == MEM) {
                compBitToZero(readMem(getRegPair(HL)), regDestCB);
                clock += 3;
            }

            // BIT b, r
            // Copies complement of specified bit b of register r to Z flag
            else {
                compBitToZero(*(regArr[regSrcCB]), regDestCB);
                clock += 2;
            }
            break;

        // ========================
        // OPCODES OF FORM 10BBBRRR
        // ========================
        case 0b10:

            // RES b, (HL)
            // Reset bit b in memory at HL to 0
            if (regSrcCB == MEM) {
                writeMem(getRegPair(HL), resetBit(readMem(getRegPair(HL)), regDestCB));
                clock += 4;
            }

            // RES b, r
            // Set bit b in register r to 0
            else {
                *(regArr[regSrcCB]) = resetBit(*(regArr[regSrcCB]), regDestCB);
                clock += 2;
            }
            break;

        // ========================
        // OPCODES OF FORM 11BBBRRR
        // ========================
        case 0b11:

            // SET b, (HL)
            // Sets bit b in memory at HL to 1
            if (regSrcCB == MEM) {
                writeMem(getRegPair(HL), setBit(readMem(getRegPair(HL)), regDestCB));
                clock += 4;
            }

            // SET b, r
            // Sets bit b in register r to 1
            else {
                *(regArr[regSrcCB]) = setBit(*(regArr[regSrcCB]), regDestCB);
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
            if (bitFive == 1) {
                condition(JUMP_REL, cond, getImm8(), 3, 2);
            }
            break;

        // INC r
        // Increment register r
        case 0b100:
            if (regDest == MEM) {
                writeMem(getRegPair(HL), inc(readMem(getRegPair(HL))));
                clock += 3;
            } else {
                *(regArr[regDest]) = inc(*(regArr[regDest]));
                clock += 1;
            }
            break;

        // DEC r
        // Decrement register r
        case 0b101:
            if (regDest == MEM) {
                writeMem(getRegPair(HL), dec(readMem(getRegPair(HL))));
                clock += 3;
            } else {
                *(regArr[regDest]) = dec(*(regArr[regDest]));
                clock += 1;
            }
            break;

        // LD r, n
        // Load 8-bit immediate n into register r
        case 0b110:
            if (regDest == MEM) {
                writeMem(getRegPair(HL), getImm8());
                clock += 3;
            } else {
                *regArr[regDest] = getImm8();
                clock += 2;
            }
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
        if (regSrc == MEM) {

            // HALT
            // stop system clock
            if (regDest == MEM) {
                halted = true;
                clock += 1;
            }

            // LD r, (HL)
            // Load memory at register pair HL into register r
            else {
                *(regArr[regDest]) = readMem(getRegPair(HL));
                clock += 2;
            }
        }

        // LD (HL), r
        // Load contents of register r into memory location
        // specified by register pair HL
        else if (regDest == MEM) {
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
            if (regSrc == MEM) {
                A = add(A, readMem(getRegPair(HL)), NO_CARRY);
                clock += 2;
                break;
            } else {
                A = add(A, *(regArr[regSrc]), NO_CARRY);
                clock += 1;
            }
            break;

        // ADC A, r
        // Add A, register r, and carry flag together
        // and store result in A
        case 0b001:
            if (regSrc == MEM) {
                A = add(A, readMem(getRegPair(HL)), WITH_CARRY);
                clock += 2;
            } else {
                A = add(A, *(regArr[regSrc]), WITH_CARRY);
                clock += 1;
            }
            break;

        // SUB r
        // Subtract register r from register A and store result in A
        case 0b010:
            if (regSrc == MEM) {
                A = sub(A, readMem(getRegPair(HL)), NO_CARRY);
                clock += 2;
            } else {
                A = sub(A, *(regArr[regSrc]), NO_CARRY);
                clock += 1;
            }
            break;

        // SBC A, r
        // Subtract register r and carry flag from
        // register A and store result in A
        case 0b011:
            if (regSrc == MEM) {
                A = sub(A, readMem(getRegPair(HL)), WITH_CARRY);
                clock += 2;
            } else {
                A = sub(A, *(regArr[regSrc]), WITH_CARRY);
                clock += 1;
            }
            break;

        // AND r
        // And register A and register r together and store result in A
        case 0b100:
            if (regSrc == MEM) {
                A = bitAnd(A, readMem(getRegPair(HL)));
                clock += 2;
            } else {
                A = bitAnd(A, *(regArr[regSrc]));
                clock += 1;
            }
            break;

        // XOR r
        // Xor register A and register r together and store result in A
        case 0b101:
            if (regSrc == MEM) {
                A = bitXor(A, readMem(getRegPair(HL)));
                clock += 2;
            } else {
                A = bitXor(A, *(regArr[regSrc]));
                clock += 1;
            }
            break;

        // OR r
        // Or register A and register r together and store result in A
        case 0b110:
            if (regSrc == MEM) {
                A = bitOr(A, readMem(getRegPair(HL)));
                clock += 2;
            } else {
                A = bitOr(A, *(regArr[regSrc]));
                clock += 1;
            }
            break;

        // CP r
        // Compare register A and register R
        case 0b111:
            if (regSrc == MEM) {
                sub(A, readMem(getRegPair(HL)), NO_CARRY);
                clock += 2;
            } else {
                sub(A, *(regArr[regSrc]), NO_CARRY);
                clock += 1;
            }
            break;
        }
        break;

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
        else if (regSrc == 0b000 && bitFive == 0) {
            condition(RETURN, cond, 0, 5, 2);
        }

        // JP cc, nn
        // Jump to address specific if condition cc is met
        else if (regSrc == 0b010 && bitFive == 0) {
            condition(JUMP, cond, getImm16(), 4, 3);
        }

        // CALL cc, nn
        // Call address at nn if condition cc is met
        else if (regSrc == 0b100 && bitFive == 0) {
            condition(CALL, cond, getImm16(), 6, 3);
        }
        break;
    }
    PC++;
}



// ======================================================================================
// ------------------------------ MEMORY ACCESS FUNCTIONS -------------------------------
// ======================================================================================

// ================================
// Read from memory
// ================================
unsigned char CPU::readMem(unsigned short addr) {
    return mem[addr];
}

// ================================
// Write to memory
// ================================
void CPU::writeMem(unsigned short addr, unsigned char value) {
    if (addr == DMA) {
        dmaTransfer(value << 8);
    } else if (addr >= ECHO_START && addr < ECHO_END) {
        mem[WORK_RAM + addr - ECHO_START] = value;
    }
    mem[addr] = value;
}

// ================================
// Push given register pair
// onto stack
// ================================
void CPU::pushRegPair(unsigned char regPair) {
    if (regPair == AF_SP) {
        push((A << 8) | getF());
    } else {
        push(getRegPair(regPair));
    }
}

// ================================
// Pop given register pair
// from stack
// ================================
void CPU::popRegPair(unsigned char regPair) {
    unsigned short popVal = pop();
    if (regPair == AF_SP) {
        A = (popVal >> 8) & 0xF;
        setF(popVal & 0xF);
    } else {
        setRegPair(regPair, popVal);
    }
}

// ================================
// Set flag register
// ================================
void CPU::setF(unsigned char value) {
    zero = value >> 7 & 0x1;
    subtract = value >> 6 & 0x1;
    halfCarry = value >> 5 & 0x1;
    carry = value >> 4 & 0x1;
}

// ================================
// Return flag register
// ================================
unsigned char CPU::getF() {
    return 0x00 | zero << 7 | subtract << 6 | halfCarry << 5 | carry << 4;
}

// ================================
// Return 8-bit immediate value
// ================================
unsigned char CPU::getImm8() {
    return readMem(++PC);
}

// ================================
// Return 16-bit immediate value
// ================================
unsigned short CPU::getImm16() {
    unsigned short imm16 = readMem(++PC);
    return imm16 | (readMem(++PC) << 8);
}

// ================================
// Set specified register pair to
// given 16-bit value
// ================================
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

// ================================
// Return specified register pair
// ================================
unsigned short CPU::getRegPair(unsigned char regPair) {
    switch (regPair) {
    case BC:
        return (B << 8) | C;
    case DE:
        return (D << 8) | E;
    case HL:
        return (H << 8) | L;
    case AF_SP:
        return SP;
    }
    return 0;
}

// ================================
// Push value onto stack
// ================================
void CPU::push(unsigned short value) {
    writeMem(--SP, (value >> 8) & 0xFF);
    writeMem(--SP, value & 0xFF);
}

// ================================
// Pop value from stack
// ================================
unsigned short CPU::pop() {
    unsigned char lo = readMem(SP++);
    return (readMem(SP++) << 8) | lo;
}

// ================================
// Set all system flags
// ================================
void CPU::setFlags(bool zeroCond, bool halfCond, bool subCond, bool carryCond) {
    zero = zeroCond;
    halfCarry = halfCond;
    carry = carryCond;
    subtract = subCond;
}

// ================================
// Save stack pointer to memory
// ================================
void CPU::saveSP() {
    unsigned short imm16 = getImm16();
    writeMem(imm16, SP & 0xFF);
    writeMem(imm16 + 1, (SP >> 8) & 0xFF);
}

// ================================
// Perform DMA transfer
// ================================
void CPU::dmaTransfer(unsigned short addr) {
    for (int i = 0; i < OAM_COUNT * BYTES_PER_OAM; i++) {
        writeMem(OAM_ADDR + i, addr + i);
    }
}



// ======================================================================================
// ------------------------------- ARITHMETIC FUNCTIONS ---------------------------------
// ======================================================================================

// ================================
// Add two 8-bit numbers together
// ================================
unsigned char CPU::add(unsigned char a, unsigned char b, bool withCarry) {
    unsigned char c = carry;
    if (!withCarry) {
        c = 0;
    }
    unsigned short result = a + b + c;
    setFlags((result & 0xFF) == 0, (a & 0xF) + (b & 0xF) + c > 0xF, false, result > 0xFF);
    return (unsigned char)result;
}

// ================================
// Add two 16-bit numbers together
// ================================
unsigned short CPU::add16(unsigned short a, unsigned short b) {
    unsigned int result = a + b;
    setFlags(zero, (a & 0xFFF) + (b & 0xFFF) > 0xFFF, false, result > 0xFFFF);
    return (unsigned short)result;
}

// ================================
// Subtract two 8-bit numbers
// ================================
unsigned char CPU::sub(unsigned char a, unsigned char b, bool withCarry) {
    unsigned char c = carry;
    if (!withCarry) {
        c = 0;
    }
    unsigned short result = a - b - c;
    setFlags(result == 0, (short)((a & 0xF) - (b  & 0xF) - c) < 0, true, (short)result < 0);
    return (unsigned char)result;
}

// ================================
// Increment given value by one
// ================================
unsigned char CPU::inc(unsigned char value) {
    unsigned char result = value + 1;
    setFlags(result == 0, (value & 0xF) + 1 > 0xF, false, carry);
    return result;
}

// ================================
// Decrement given value by one
// ================================
unsigned char CPU::dec(unsigned char value) {
    unsigned char result = value - 1;
    setFlags(result == 0, (value & 0xF) - 1 < 0, true, carry);
    return result;
}

// ================================
// Decimal adjust accumulator
// after add or subtract operation
// ================================
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



// ======================================================================================
// --------------------------------- LOGICAL FUNCTIONS ----------------------------------
// ======================================================================================

// ================================
// AND two 8-bit numbers together
// ================================
unsigned char CPU::bitAnd(unsigned char a, unsigned char b) {
    unsigned char result = a & b;
    setFlags(result == 0, true, false, false);
    return result;
}

// ================================
// OR two 8-bit numbers together
// ================================
unsigned char CPU::bitOr(unsigned char a, unsigned char b) {
    unsigned char result = a | b;
    setFlags(result == 0, halfCarry, subtract, carry);
    return result;
}

// ================================
// XOR two 8-bit numbers together
// ================================
unsigned char CPU::bitXor(unsigned char a, unsigned char b) {
    unsigned char result = a ^ b;
    setFlags(result == 0, halfCarry, subtract, carry);
    return result;
}

// ================================
// Rotate given value to the left
// ================================
unsigned char CPU::rotateLeft(unsigned char value) {
    unsigned char bit7 = (value & 0x80) >> 7;
    unsigned char result = (value << 1) | bit7;
    setFlags(result == 0, false, false, bit7);
    return result;
}

// ================================
// Rotate given value to the left
// through carry flag
// ================================
unsigned char CPU::rotateLeftCarry(unsigned char value) {
    unsigned char result = (value << 1) | (unsigned char)carry;
    setFlags(result == 0, false, false, (value & 0x80) >> 7);
    return result;
}

// ================================
// Rotate given value to the right
// ================================
unsigned char CPU::rotateRight(unsigned char value) {
    unsigned char result = (value >> 1) | ((value & 0x1) << 7);
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Rotates value to the right
// through carry flag
// ================================
unsigned char CPU::rotateRightCarry(unsigned char value) {
    unsigned char result = (value >> 1) | (carry << 7);
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Shift given value to the left
// ================================
unsigned char CPU::shiftLeft(unsigned char value) {
    unsigned char result = value << 1;
    setFlags(result == 0, false, false, (value & 0x80) >> 7);
    return result;
}

// ================================
// Arithmetically shift given value
// to the right
// ================================
unsigned char CPU::shiftRightArithmetic(unsigned char value) {
    unsigned char msb = value & 0x80;
    unsigned char result = (value >> 1) | msb;
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Logically shift given value to
// the right by one
// ================================
unsigned char CPU::shiftRightLogical(unsigned char value) {
    unsigned char result = (value >> 1) & 0x7F;
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Swap lower and upper nibbles of
// a given value
// ================================
unsigned char CPU::swap(unsigned char value) {
    unsigned char result = ((value << 4) & 0xF0) | ((value >> 4) & 0xF);
    setFlags(result == 0, false, false, false);
    return result;
}

// ================================
// Copy complement of specified bit
// of a given value into zero flag
// ================================
void CPU::compBitToZero(unsigned char value, unsigned char bit) {
    zero = !(bool)((value >> bit) & 0x1);
    setFlags(zero, true, false, carry);
}

// ================================
// Set specified bit to 1
// ================================
unsigned char CPU::setBit(unsigned char value, unsigned int bit) {
    return value | (1 << bit);
}

// ================================
// Set specified bit to 0
// ================================
unsigned char CPU::resetBit(unsigned char value, unsigned int bit) {
    return value & ~(1 << bit);
}



// ======================================================================================
// ------------------------------ CONTROL FLOW FUNCTIONS --------------------------------
// ======================================================================================

// ================================
// Jump to given address
// ================================
void CPU::jump(unsigned short addr) {
    PC = addr - 1;
}

// ================================
// Jump to address relative
// to given 8-bit value
// ================================
void CPU::jumpRel(char addrRel) {
    PC += (short)addrRel;
}

// ================================
// Call subroutine at given address
// ================================
void CPU::call(unsigned short addr) {
    push(PC + 1);
    jump(addr);
}

// ================================
// Return from current subroutine
// ================================
void CPU::ret() {
    PC = pop() - 1;
}

// ================================
// Performs speficied action if a
// given condition is met
// ================================
void CPU::condition(Control condFunc, unsigned char condValue,
                    unsigned short imm, int clockSuccess, int clockFail) {

    // get condition
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

    // run condition function is condition holds
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



// ======================================================================================
// ------------------------------- INTERRUPT FUNCTIONS ----------------------------------
// ======================================================================================

void CPU::checkForInt() {
    if (halted && (mem[IF] & mem[IE]) != 0) {
        halted = false;
    }

    if (IME) {
        unsigned short intAddr = 0;

        if (vblankIntTriggered()) {
            intAddr = 0x40;
            mem[IF] &= 0xFE;
        } else if (lcdIntTriggered()) {
            intAddr = 0x48;
            mem[IF] &= 0xFD;
        } else if (timerIntTriggered()) {
            intAddr = 0x50;
            mem[IF] &= 0xFB;
        } else if (serialIntTriggered()) {
            intAddr = 0x58;
            mem[IF] &= 0xF7;
        } else if (joypadIntTriggered()) {
            intAddr = 0x60;
            mem[IF] &= 0xEF;
        }

        if (intAddr != 0) {
            IME = false;
            push(PC);
            PC = intAddr;
            clock += 3;
        }
    }
}

bool CPU::vblankIntTriggered() {
    return (mem[IE] & mem[IF]) & 1;
}

bool CPU::lcdIntTriggered() {
    return ((mem[IE] & mem[IF]) >> 1) & 1;
}

bool CPU::timerIntTriggered() {
    return ((mem[IE] & mem[IF]) >> 2) & 1;
}

bool CPU::serialIntTriggered() {
    return ((mem[IE] & mem[IF]) >> 3) & 1;
}

bool CPU::joypadIntTriggered() {
    return ((mem[IE] & mem[IF]) >> 4) & 1;
}