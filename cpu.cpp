#include "cpu.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

// ================================
// Initialize cpu data
// ================================
CPU::CPU() {
    mmu = new MMU();
    powerUp();
}

void CPU::powerUp() {

    // set registers
    A = H = 0x01;
    B = D = 0x00;
    C = 0x13;
    E = 0xD8;
    L = 0x4D;
    PC = 0x100;
    SP = 0xFFFE;

    // set flags
    zero = halfCarry = carry = true;
    subtract = false;
    IME = false;

    // set memory
    mmu->write(0xFF10, 0x80);
    mmu->write(0xFF11, 0xBF);
    mmu->write(0xFF12, 0xF3);
    mmu->write(0xFF14, 0xBF);
    mmu->write(0xFF16, 0x3F);
    mmu->write(0xFF17, 0x00);
    mmu->write(0xFF19, 0xBF);
    mmu->write(0xFF1A, 0x7F);
    mmu->write(0xFF1B, 0xFF);
    mmu->write(0xFF1C, 0x9F);
    mmu->write(0xFF1E, 0xBF);
    mmu->write(0xFF20, 0xFF);
    mmu->write(0xFF21, 0x00);
    mmu->write(0xFF22, 0x00);
    mmu->write(0xFF23, 0xBF);
    mmu->write(0xFF24, 0x77);
    mmu->write(0xFF25, 0xF3);
    mmu->write(0xFF26, 0xF1);
    mmu->write(LCDC, 0x91);
    mmu->write(SCROLL_Y, 0x00);
    mmu->write(SCROLL_X, 0x00);
    mmu->write(LY_COMP, 0x00);
    mmu->write(BGP, 0xFC);
    mmu->write(OBP0, 0xFF);
    mmu->write(OBP1, 0xFF);
    mmu->write(WINDOW_Y, 0x00);
    mmu->write(WINDOW_X, 0x00);
    mmu->write(IE, 0x00);

    // extra memory settings
    mmu->write(STAT, 0x80);
    mmu->write(IF, 0xE1);

    // zero memory from 0x8000 to 0x9FFF
    for (int i = VRAM_START; i < VRAM_END; i++) {
        mmu->mem[i] = 0;
    }

    // zero RAM
    for (unsigned int i = 0; i < EXT_RAM_SIZE; i++) {
        mmu->ram[i] = 0;
    }

    // other values
    halted = false;
    cycleCount = 0;
    internalDivider = 0;
    internalCounter = 0;
    mmu->bankLowerBits = 1;
    mmu->bankUpperBits = 0;
}

// ================================
// Step through emulator cycle
// ================================
void CPU::step() {
    cycleCount = 0;
    checkForInt();
    if (!halted) {
        decode(mmu->read(PC));
    } else {
        cycleCount = 1;
    }
    incTimers();
}

// ================================
// Increment timers
// ================================
void CPU::incTimers() {
    internalDivider += cycleCount;
    while (internalDivider >= DIVIDER_CYCLES) {
        mmu->mem[DIV]++;
        internalDivider -= DIVIDER_CYCLES;
    }

    if (mmu->read(TAC) & 0x4) {
        int counterMod = 0;
        switch (mmu->read(TAC) & 0b11) {
            case 0b00:
                counterMod = COUNTER_CYCLES_0;
                break;
            case 0b01:
                counterMod = COUNTER_CYCLES_1;
                break;
            case 0b10:
                counterMod = COUNTER_CYCLES_2;
                break;
            case 0b11:
                counterMod = COUNTER_CYCLES_3;
                break;
        }

        internalCounter += cycleCount;
        while (internalCounter >= counterMod) {
            mmu->write(TIMA, mmu->read(TIMA) + 1);
            internalCounter -= counterMod;

            if (mmu->read(TIMA) == 0) {
                mmu->write(TIMA, mmu->read(TMA));
                setInt(TIMER_OVERFLOW);
            }
        }
    }
}

// ================================
// Decode instruction
// ================================
void CPU::decode(unsigned char opcode) {

    // values for single byte opcodes
    unsigned char upperTwo = (opcode >> 6) & 0b11;
    unsigned char bitFive = (opcode >> 5) & 0x1;
    unsigned char cond = (opcode >> 3) & 0b11;
    unsigned char regDest = (opcode >> 3) & 0b111;
    unsigned char regSrc = opcode & 0b111;
    unsigned char lo = opcode & 0xF;
    unsigned char regPair = (opcode >> 4) & 0b11;

    // values for cb prefix opcode
    unsigned char imm8 = mmu->read(PC + 1);
    unsigned char upperTwoCB = (imm8 >> 6) & 0b11;
    unsigned char regDestCB = (imm8 >> 3) & 0b111;
    unsigned char regSrcCB = imm8 & 0b111;

    // ======================================================================================
    // ---------------------------- OPCODES IN NON-GENERAL FORM -----------------------------
    // ======================================================================================
    switch (opcode) {

        // LD A, (BC)
        // Load memory at address BC into register A
        case 0x0A:
            A = mmu->read(getRegPair(BC));
            cycleCount += 2;
            break;

        // LD A, (DE)
        // Load memory at address DE into register A
        case 0x1A:
            A = mmu->read(getRegPair(DE));
            cycleCount += 2;
            break;

        // LD A, (C)
        // Load memory at address 0xFF00 + [register C] into register A
        case 0xF2:
            A = mmu->read(0xFF00 + C);
            cycleCount += 2;
            break;

        // LD (C), A
        // Load register A into memory at address 0xFF00 + [register C]
        case 0xE2:
            mmu->write(0xFF00 + C, A);
            cycleCount += 2;
            break;

        // LD A, (n)
        // Load memory at address 0xFF00 + [8-bit immediate n] into register A
        case 0xF0:
            A = mmu->read(0xFF00 + getImm8());
            cycleCount += 3;
            break;

        // LD (n), A
        // Load register A into memory at address 0xFF00 + [8-bit immediate n]
        case 0xE0:
            mmu->write(0xFF00 + getImm8(), A);
            cycleCount += 3;
            break;

        // LD A, (nn)
        // Load memory at address specified by 16-bit immediate nn into register A
        case 0xFA:
            A = mmu->read(getImm16());
            cycleCount += 4;
            break;

        // LD (nn), A
        // Load register A into memory at address specified by 16-bit immediate nn
        case 0xEA:
            mmu->write(getImm16(), A);
            cycleCount += 4;
            break;

        // LD A, (HLI)
        // Load memory at address HL into register A, then increment HL
        case 0x2A:
            A = mmu->read(getRegPair(HL));
            setRegPair(HL, getRegPair(HL) + 1);
            cycleCount += 2;
            break;

        // LD A, (HLD)
        // Load memory at address HL into register A, then decrement HL
        case 0x3A:
            A = mmu->read(getRegPair(HL));
            setRegPair(HL, getRegPair(HL) - 1);
            cycleCount += 2;
            break;

        // LD (BC), A
        // Load register A into memory at address BC
        case 0x02:
            mmu->write(getRegPair(BC), A);
            cycleCount += 2;
            break;

        // LD (DE), A
        // Load register A into memory at address DE
        case 0x12:
            mmu->write(getRegPair(DE), A);
            cycleCount += 2;
            break;

        // LD (HLI), A
        // Load register A into memory at address HL, then increment HL
        case 0x22:
            mmu->write(getRegPair(HL), A);
            setRegPair(HL, getRegPair(HL) + 1);
            cycleCount += 2;
            break;

        // LD (HLD), A
        // Load register A into memory at address HL, then decrement HL
        case 0x32:
            mmu->write(getRegPair(HL), A);
            setRegPair(HL, getRegPair(HL) - 1);
            cycleCount += 2;
            break;

        // LD SP, HL
        // Load register HL into SP
        case 0xF9:
            SP = getRegPair(HL);
            cycleCount += 2;
            break;

        // LDHL SP, e
        // Add 8-bit immediate e to SP and store result in HL
        case 0xF8:
            setRegPair(HL, addSPImm8((char)getImm8()));
            zero = false;
            cycleCount += 3;
            break;

        // LD (nn), SP
        // Load stack pointer into memory at address specified by 16-bit immediate nn
        case 0x08:
            saveSP();
            cycleCount += 5;
            break;

        // ADD A, n
        // Add 8-bit immediate n with register A and store result in A
        case 0xC6:
            A = add(A, getImm8(), NO_CARRY);
            cycleCount += 2;
            break;

        // ADC A, n
        // Add A, 8-bit immediate n, and carry flag together and store result in A
        case 0xCE:
            A = add(A, getImm8(), WITH_CARRY);
            cycleCount += 2;
            break;

        // SUB n
        // Subtract 8-bit immediate n from register A and store result in A
        case 0xD6:
            A = sub(A, getImm8(), NO_CARRY);
            cycleCount += 2;
            break;

        // SBC A, n
        // Subtract 8-bit immediate n and carry flag from register A and store result in A
        case 0xDE:
            A = sub(A, getImm8(), WITH_CARRY);
            cycleCount += 2;
            break;

        // AND n
        // And register A and 8-bit immediate n together and store result in A
        case 0xE6:
            A = bitAnd(A, getImm8());
            cycleCount += 2;
            break;

        // OR n
        // Or register A and 8-bit immediate n together and store result in A
        case 0xF6:
            A = bitOr(A, getImm8());
            cycleCount += 2;
            break;

        // XOR n
        // Xor register A and 8-bit immediate n together and store result in A
        case 0xEE:
            A = bitXor(A, getImm8());
            cycleCount += 2;
            break;

        // CP n
        // Compare register A and 8-bit immediate n
        case 0xFE:
            sub(A, getImm8(), NO_CARRY);
            cycleCount += 2;
            break;

        // ADD SP, e
        // Add 8-bit immediate e to SP and store result in SP
        case 0xE8:
            SP = addSPImm8((char)getImm8());
            zero = false;
            cycleCount += 4;
            break;

        // RLCA
        // Rotate register A to the left
        case 0x07:
            A = rotateLeft(A);
            zero = false;
            cycleCount += 1;
            break;

        // RLA
        // Rotate register A to the left through the carry flag
        case 0x17:
            A = rotateLeftCarry(A);
            zero = false;
            cycleCount += 1;
            break;

        // RRCA
        // Rotate register A to the right
        case 0x0F:
            A = rotateRight(A);
            zero = false;
            cycleCount += 1;
            break;

        // RRA
        // Rotate register A to the right through the carry flag
        case 0x1F:
            A = rotateRightCarry(A);
            zero = false;
            cycleCount += 1;
            break;

        // JP nn
        // Jump to address specified by 16 bit immediate
        case 0xC3:
            jump(getImm16());
            cycleCount += 4;
            break;

        // JR e
        // Jump to PC + (signed) e
        case 0x18:
            jumpRel((char)getImm8());
            cycleCount += 3;
            break;

        // JP (HL)
        // Jump to address specified by register HL
        case 0xE9:
            jump(getRegPair(HL));
            cycleCount += 1;
            break;

        // CALL nn
        // Push current address onto stack and jump to address specified by 16 bit immediate
        case 0xCD:
            call(getImm16());
            cycleCount += 6;
            break;

        // RET
        // Pop stack and return to popped address
        case 0xC9:
            ret();
            cycleCount += 4;
            break;

        // RETI
        // Return from interrupt routine
        case 0xD9:
            ret();
            cycleCount += 4;
            IME = true;
            break;

        // RST t
        // Load zero page memory address specified by t into PC
        case 0xC7:
            call(0x00);
            cycleCount += 4;
            break;
        case 0xCF:
            call(0x08);
            cycleCount += 4;
            break;
        case 0xD7:
            call(0x10);
            cycleCount += 4;
            break;
        case 0xDF:
            call(0x18);
            cycleCount += 4;
            break;
        case 0xE7:
            call(0x20);
            cycleCount += 4;
            break;
        case 0xEF:
            call(0x28);
            cycleCount += 4;
            break;
        case 0xF7:
            call(0x30);
            cycleCount += 4;
            break;
        case 0xFF:
            call(0x38);
            cycleCount += 4;
            break;

        // DAA
        // Decimal adjust accumulator
        case 0x27:
            decimalAdjustAcc();
            cycleCount += 1;
            break;

        // CPL
        // Complement register A
        case 0x2F:
            A = ~A;
            halfCarry = true;
            subtract = true;
            cycleCount += 1;
            break;

        // NOP
        // No operation
        case 0x00:
            cycleCount += 1;
            break;

        // CCF
        // Complement carry flag
        case 0x3F:
            carry = !carry;
            halfCarry = false;
            subtract = false;
            cycleCount += 1;
            break;

        // SCF
        // Set carry flag
        case 0x37:
            carry = true;
            halfCarry = false;
            subtract = false;
            cycleCount += 1;
            break;

        // DI
        // Reset interrupt master enable flag
        case 0xF3:
            IME = false;
            cycleCount += 1;
            break;

        // EI
        // Set interrupt master enable flag
        case 0xFB:
            IME = true;
            cycleCount += 1;
            break;

        // STOP
        // Stop system clock and oscillator circuit
        // TODO: implement stop instruction
        case 0x10:
            cycleCount += 1;
            break;



        // ======================================================================================
        // --------------------------------- CB PREFIX OPCODES ----------------------------------
        // ======================================================================================
        case 0xCB:
            PC++;

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
                                mmu->write(getRegPair(HL), rotateLeft(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = rotateLeft(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // RRC r
                        // Rotate register r to the right
                        case 0b001:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), rotateRight(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = rotateRight(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // RL r
                        // Rotate register r to the left through carry flag
                        case 0b010:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), rotateLeftCarry(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = rotateLeftCarry(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // RR r
                        // Rotate register r to the right through carry flag
                        case 0b011:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), rotateRightCarry(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = rotateRightCarry(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // SLA r
                        // Shift register r to the left
                        case 0b100:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), shiftLeft(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = shiftLeft(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // SRA r
                        // Shift register r to the right
                        case 0b101:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), shiftRightArithmetic(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = shiftRightArithmetic(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // SWAP r
                        // Swap upper and lower nibbles of register r
                        case 0b110:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), swap(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = swap(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        // SRL r
                        // Shift register r to the right logically
                        case 0b111:
                            if (regSrcCB == MEM) {
                                mmu->write(getRegPair(HL), shiftRightLogical(mmu->read(getRegPair(HL))));
                                cycleCount += 4;
                            } else {
                                *(regArr[regSrcCB]) = shiftRightLogical(*(regArr[regSrcCB]));
                                cycleCount += 2;
                            }
                            break;

                        default:
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
                        compBitToZero(mmu->read(getRegPair(HL)), regDestCB);
                        cycleCount += 3;
                    }

                    // BIT b, r
                    // Copies complement of specified bit b of register r to Z flag
                    else {
                        compBitToZero(*(regArr[regSrcCB]), regDestCB);
                        cycleCount += 2;
                    }
                    break;

                // ========================
                // OPCODES OF FORM 10BBBRRR
                // ========================
                case 0b10:

                    // RES b, (HL)
                    // Reset bit b in memory at HL to 0
                    if (regSrcCB == MEM) {
                        mmu->write(getRegPair(HL), resetBit(mmu->read(getRegPair(HL)), regDestCB));
                        cycleCount += 4;
                    }

                    // RES b, r
                    // Set bit b in register r to 0
                    else {
                        *(regArr[regSrcCB]) = resetBit(*(regArr[regSrcCB]), regDestCB);
                        cycleCount += 2;
                    }
                    break;

                // ========================
                // OPCODES OF FORM 11BBBRRR
                // ========================
                case 0b11:

                    // SET b, (HL)
                    // Sets bit b in memory at HL to 1
                    if (regSrcCB == MEM) {
                        mmu->write(getRegPair(HL), setBit(mmu->read(getRegPair(HL)), regDestCB));
                        cycleCount += 4;
                    }

                    // SET b, r
                    // Sets bit b in register r to 1
                    else {
                        *(regArr[regSrcCB]) = setBit(*(regArr[regSrcCB]), regDestCB);
                        cycleCount += 2;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
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
                        mmu->write(getRegPair(HL), inc(mmu->read(getRegPair(HL))));
                        cycleCount += 3;
                    } else {
                        *(regArr[regDest]) = inc(*(regArr[regDest]));
                        cycleCount += 1;
                    }
                    break;

                // DEC r
                // Decrement register r
                case 0b101:
                    if (regDest == MEM) {
                        mmu->write(getRegPair(HL), dec(mmu->read(getRegPair(HL))));
                        cycleCount += 3;
                    } else {
                        *(regArr[regDest]) = dec(*(regArr[regDest]));
                        cycleCount += 1;
                    }
                    break;

                // LD r, n
                // Load 8-bit immediate n into register r
                case 0b110:
                    if (regDest == MEM) {
                        mmu->write(getRegPair(HL), getImm8());
                        cycleCount += 3;
                    } else {
                        *regArr[regDest] = getImm8();
                        cycleCount += 2;
                    }
                    break;

                default:
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
                    cycleCount += 3;
                    break;

                // INC ss
                // Increment register ss
                case 0x3:
                    setRegPair(regPair, getRegPair(regPair) + 1);
                    cycleCount += 2;
                    break;

                // ADD HL, ss
                // Add register HL and register ss and store store result in HL
                case 0x9:
                    setRegPair(HL, add16(getRegPair(HL), getRegPair(regPair)));
                    cycleCount += 2;
                    break;

                // DEC ss
                // Decrement register ss
                case 0xB:
                    setRegPair(regPair, getRegPair(regPair) - 1);
                    cycleCount += 2;
                    break;

                default:
                    break;
            }
            break;

        case 0b01:
            if (regSrc == MEM) {

                // HALT
                // stop system clock
                if (regDest == MEM) {
                    if (IME || ((mmu->read(IE) & mmu->read(IF)) & 0x1F) == 0) {
                        halted = true;
                    } else {
                        printf("HALT BUG");
                    }
                    cycleCount += 1;
                }

                // LD r, (HL)
                // Load memory at register pair HL into register r
                else {
                    *(regArr[regDest]) = mmu->read(getRegPair(HL));
                    cycleCount += 2;
                }
            }

            // LD (HL), r
            // Load contents of register r into memory location
            // specified by register pair HL
            else if (regDest == MEM) {
                mmu->write(getRegPair(HL), *(regArr[regSrc]));
                cycleCount += 2;
            }

            // LD r, r'
            // Load contents of register r' into register r
            else {
                *(regArr[regDest]) = *(regArr[regSrc]);
                cycleCount += 1;
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
                        A = add(A, mmu->read(getRegPair(HL)), NO_CARRY);
                        cycleCount += 2;
                        break;
                    } else {
                        A = add(A, *(regArr[regSrc]), NO_CARRY);
                        cycleCount += 1;
                    }
                    break;

                // ADC A, r
                // Add A, register r, and carry flag together
                // and store result in A
                case 0b001:
                    if (regSrc == MEM) {
                        A = add(A, mmu->read(getRegPair(HL)), WITH_CARRY);
                        cycleCount += 2;
                    } else {
                        A = add(A, *(regArr[regSrc]), WITH_CARRY);
                        cycleCount += 1;
                    }
                    break;

                // SUB r
                // Subtract register r from register A and store result in A
                case 0b010:
                    if (regSrc == MEM) {
                        A = sub(A, mmu->read(getRegPair(HL)), NO_CARRY);
                        cycleCount += 2;
                    } else {
                        A = sub(A, *(regArr[regSrc]), NO_CARRY);
                        cycleCount += 1;
                    }
                    break;

                // SBC A, r
                // Subtract register r and carry flag from
                // register A and store result in A
                case 0b011:
                    if (regSrc == MEM) {
                        A = sub(A, mmu->read(getRegPair(HL)), WITH_CARRY);
                        cycleCount += 2;
                    } else {
                        A = sub(A, *(regArr[regSrc]), WITH_CARRY);
                        cycleCount += 1;
                    }
                    break;

                // AND r
                // And register A and register r together and store result in A
                case 0b100:
                    if (regSrc == MEM) {
                        A = bitAnd(A, mmu->read(getRegPair(HL)));
                        cycleCount += 2;
                    } else {
                        A = bitAnd(A, *(regArr[regSrc]));
                        cycleCount += 1;
                    }
                    break;

                // XOR r
                // Xor register A and register r together and store result in A
                case 0b101:
                    if (regSrc == MEM) {
                        A = bitXor(A, mmu->read(getRegPair(HL)));
                        cycleCount += 2;
                    } else {
                        A = bitXor(A, *(regArr[regSrc]));
                        cycleCount += 1;
                    }
                    break;

                // OR r
                // Or register A and register r together and store result in A
                case 0b110:
                    if (regSrc == MEM) {
                        A = bitOr(A, mmu->read(getRegPair(HL)));
                        cycleCount += 2;
                    } else {
                        A = bitOr(A, *(regArr[regSrc]));
                        cycleCount += 1;
                    }
                    break;

                // CP r
                // Compare register A and register R
                case 0b111:
                    if (regSrc == MEM) {
                        sub(A, mmu->read(getRegPair(HL)), NO_CARRY);
                        cycleCount += 2;
                    } else {
                        sub(A, *(regArr[regSrc]), NO_CARRY);
                        cycleCount += 1;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 0b11:

            // PUSH qq
            // Push register pair qq onto stack
            if (lo == 0x5) {
                pushRegPair(regPair);
                cycleCount += 4;
            }

            // POP qq
            // Pop contents of stack into register pair qq
            else if (lo == 0x1) {
                popRegPair(regPair);
                cycleCount += 3;
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

        default:
            break;
    }
    PC++;
}



// ======================================================================================
// ------------------------------ MEMORY ACCESS FUNCTIONS -------------------------------
// ======================================================================================

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
        A = popVal >> 8;
        setF(popVal);
    } else {
        setRegPair(regPair, popVal);
    }
}

// ================================
// Set flag register
// ================================
void CPU::setF(unsigned char value) {
    zero = (value >> 7) & 0x1;
    subtract = (value >> 6) & 0x1;
    halfCarry = (value >> 5) & 0x1;
    carry = (value >> 4) & 0x1;
}

// ================================
// Return flag register
// ================================
unsigned char CPU::getF() const {
    return (zero << 7) | (subtract << 6) | (halfCarry << 5) | (carry << 4);
}

// ================================
// Return 8-bit immediate value
// ================================
unsigned char CPU::getImm8() {
    return mmu->read(++PC);
}

// ================================
// Return 16-bit immediate value
// ================================
unsigned short CPU::getImm16() {
    unsigned short imm16 = mmu->read(++PC);
    return imm16 | (mmu->read(++PC) << 8);
}

// ================================
// Set specified register pair to
// given 16-bit value
// ================================
void CPU::setRegPair(unsigned char regPair, unsigned short value) {
    switch (regPair) {
        case BC:
            B = value >> 8;
            C = value;
            break;
        case DE:
            D = value >> 8;
            E = value;
            break;
        case HL:
            H = value >> 8;
            L = value;
            break;
        case AF_SP:
            SP = value;
            break;
        default:
            break;
    }
}

// ================================
// Return specified register pair
// ================================
unsigned short CPU::getRegPair(unsigned char regPair) const {
    switch (regPair) {
        case BC:
            return (B << 8) | C;
        case DE:
            return (D << 8) | E;
        case HL:
            return (H << 8) | L;
        case AF_SP:
            return SP;
        default:
            break;
    }
    return 0;
}

// ================================
// Push value onto stack
// ================================
void CPU::push(unsigned short value) {
    mmu->write(--SP, value >> 8);
    mmu->write(--SP, value);
}

// ================================
// Pop value from stack
// ================================
unsigned short CPU::pop() {
    unsigned char lo = mmu->read(SP++);
    return (mmu->read(SP++) << 8) | lo;
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
    mmu->write(imm16, SP);
    mmu->write(imm16 + 1, SP >> 8);
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
    return result;
}

// ================================
// Add two 16-bit numbers together
// ================================
unsigned short CPU::add16(unsigned short a, unsigned short b) {
    unsigned int result = a + b;
    setFlags(zero, (a & 0xFFF) + (b & 0xFFF) > 0xFFF, false, result > 0xFFFF);
    return result;
}

unsigned short CPU::addSPImm8(char imm8) {
    unsigned short result = SP + imm8;
    if (imm8 >= 0) {
        setFlags(false, (SP & 0xF) + (imm8 & 0xF) > 0xF, false, (SP & 0xFF) + imm8 > 0xFF);
    } else {
        setFlags(false, (result & 0xF) <= (SP & 0xF), false, (result & 0xFF) <= (SP & 0xFF));
    }
    return result;
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
    setFlags((result & 0xFF) == 0, ((a & 0xF) - (b  & 0xF) - c) < 0, true, (short)result < 0);
    return result;
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
    unsigned char correction = 0;

    if (halfCarry || (!subtract && (A & 0xF) > 0x9)) {
        correction |= 0x6;
    }

    if (carry || (!subtract && A > 0x99)) {
        correction |= 0x60;
        carry = true;
    }

    if (!subtract) {
        A += correction;
    } else {
        A -= correction;
    }

    zero = A == 0;
    halfCarry = false;
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
    setFlags(result == 0, false, false, false);
    return result;
}

// ================================
// XOR two 8-bit numbers together
// ================================
unsigned char CPU::bitXor(unsigned char a, unsigned char b) {
    unsigned char result = a ^ b;
    setFlags(result == 0, false, false, false);
    return result;
}

// ================================
// Rotate given value to the left
// ================================
unsigned char CPU::rotateLeft(unsigned char value) {
    unsigned char bit7 = ((value & 0x80) >> 7) & 0x1;
    unsigned char result = (value << 1) | bit7;
    setFlags(result == 0, false, false, bit7);
    return result;
}

// ================================
// Rotate given value to the left
// through carry flag
// ================================
unsigned char CPU::rotateLeftCarry(unsigned char value) {
    unsigned char result = (value << 1) | carry;
    setFlags(result == 0, false, false, ((value & 0x80) >> 7) & 0x1);
    return result;
}

// ================================
// Rotate given value to the right
// ================================
unsigned char CPU::rotateRight(unsigned char value) {
    unsigned char result = ((value >> 1) & 0x7F) | ((value & 0x1) << 7);
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Rotates value to the right
// through carry flag
// ================================
unsigned char CPU::rotateRightCarry(unsigned char value) {
    unsigned char result = ((value >> 1) & 0x7F) | (carry << 7);
    setFlags(result == 0, false, false, value & 0x1);
    return result;
}

// ================================
// Shift given value to the left
// ================================
unsigned char CPU::shiftLeft(unsigned char value) {
    unsigned char result = value << 1;
    setFlags(result == 0, false, false, ((value & 0x80) >> 7) & 0x1);
    return result;
}

// ================================
// Arithmetically shift given value
// to the right
// ================================
unsigned char CPU::shiftRightArithmetic(unsigned char value) {
    unsigned char msb = value & 0x80;
    unsigned char result = ((value >> 1) & 0x7F) | msb;
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
    setFlags(!(bool)((value >> bit) & 0x1), true, false, carry);
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
// Performs specified action if a
// given condition is met
// ================================
void CPU::condition(Control condFunc, unsigned char condValue,
                    unsigned short imm, int clockSuccess, int clockFail) {

    // get condition
    bool cond = false;
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
        default:
            break;
    }

    // run condition function is condition holds
    if (cond) {
        switch (condFunc) {
            case JUMP:
                jump(imm);
                break;
            case JUMP_REL:
                jumpRel((char)imm);
                break;
            case CALL:
                call(imm);
                break;
            case RETURN:
                ret();
                break;
        }
        cycleCount += clockSuccess;
    } else {
        cycleCount += clockFail;
    }
}



// ======================================================================================
// ------------------------------- INTERRUPT FUNCTIONS ----------------------------------
// ======================================================================================

void CPU::setInt(Interrupt i) const {
    switch (i) {
        case VBLANK:
            mmu->write(IF, mmu->read(IF) | VBLANK_INT_SET);
            break;
        case LCD_STAT:
            mmu->write(IF, mmu->read(IF) | LCD_INT_SET);
            break;
        case TIMER_OVERFLOW:
            mmu->write(IF, mmu->read(IF) | TIMER_INT_SET);
            break;
        case SERIAL_LINK:
            mmu->write(IF, mmu->read(IF) | SERIAL_INT_SET);
            break;
        case JOYPAD_PRESSED:
            mmu->write(IF, mmu->read(IF) | JOYPAD_INT_SET);
            break;
    }
}

void CPU::resetInt(Interrupt i) const {
    switch(i) {
        case VBLANK:
            mmu->write(IF, mmu->read(IF) & VBLANK_INT_RESET);
            break;
        case LCD_STAT:
            mmu->write(IF, mmu->read(IF) & LCD_INT_RESET);
            break;
        case TIMER_OVERFLOW:
            mmu->write(IF, mmu->read(IF) & TIMER_INT_RESET);
            break;
        case SERIAL_LINK:
            mmu->write(IF, mmu->read(IF) & SERIAL_INT_RESET);
            break;
        case JOYPAD_PRESSED:
            mmu->write(IF, mmu->read(IF) & JOYPAD_INT_RESET);
            break;
    }
}

void CPU::checkForInt() {
    if (halted && (mmu->read(IF) & mmu->read(IE) & 0x1F) != 0) {
        halted = false;
        cycleCount += 1;
    }

    if (IME) {
        unsigned short intAddr = 0;

        if (vblankIntTriggered()) {
            intAddr = 0x40;
            resetInt(VBLANK);
        } else if (lcdIntTriggered()) {
            intAddr = 0x48;
            resetInt(LCD_STAT);
        } else if (timerIntTriggered()) {
            intAddr = 0x50;
            resetInt(TIMER_OVERFLOW);
        } else if (serialIntTriggered()) {
            intAddr = 0x58;
            mmu->write(SC, mmu->read(SC) & 0x7F);
            resetInt(SERIAL_LINK);
        } else if (joypadIntTriggered()) {
            intAddr = 0x60;
            resetInt(JOYPAD_PRESSED);
        }

        if (intAddr != 0) {
            IME = false;
            push(PC);
            PC = intAddr;
            cycleCount += 5;
        }
    }
}

bool CPU::vblankIntTriggered() const {
    return mmu->read(IE) & mmu->read(IF) & VBLANK_INT_SET;
}

bool CPU::lcdIntTriggered() const {
    return mmu->read(IE) & mmu->read(IF) & LCD_INT_SET;
}

bool CPU::timerIntTriggered() const {
    return mmu->read(IE) & mmu->read(IF) & TIMER_INT_SET;
}

bool CPU::serialIntTriggered() const {
    return (mmu->read(IE) & mmu->read(IF) & SERIAL_INT_SET) || ((mmu->read(SC) & 0x81) == 0x81);
}

bool CPU::joypadIntTriggered() const {
    return mmu->read(IE) & mmu->read(IF) & JOYPAD_INT_SET;
}

#pragma clang diagnostic pop