#include <stdint-gcc.h>

class Gameboy {
public:

    uint16_t AF, BC, DE, HL, PC, SP;
    uint8_t mem[0x8000];

    Gameboy() {
        AF = 0;
        BC = 0;
        DE = 0;
        HL = 0;
        PC = 0x100;
        SP = 0xFFFE;
        for (unsigned char & i : mem) i = 0;
    }

    void decode() {

        switch(mem[PC]) {

            // 8-Bit Transfer and Input/Output Instructions

            /*
             * LD r, r'
             * Load contents of register r' into register r
             */

            // destination A
            case 0x7F:
                setA(getA());
                break;
            case 0x78:
                setA(getB());
                break;
            case 0x79:
                setA(getC());
                break;
            case 0x7A:
                setA(getD());
                break;
            case 0x7B:
                setA(getE());
                break;
            case 0x7C:
                setA(getH());
                break;
            case 0x7D:
                setA(getL());
                break;
            case 0x47:
                setB(getA());
                break;

            // destination B
            case 0x40:
                setB(getB());
                break;
            case 0x41:
                setB(getC());
                break;
            case 0x42:
                setB(getD());
                break;
            case 0x43:
                setB(getE());
                break;
            case 0x44:
                setB(getH());
                break;
            case 0x45:
                setB(getL());
                break;

            // destination C
            case 0x4F:
                setC(getA());
                break;
            case 0x48:
                setC(getB());
                break;
            case 0x49:
                setC(getC());
                break;
            case 0x4A:
                setC(getD());
                break;
            case 0x4B:
                setC(getE());
                break;
            case 0x4C:
                setC(getH());
                break;
            case 0x4D:
                setC(getL());
                break;

            // destination D
            case 0x57:
                setD(getA());
                break;
            case 0x50:
                setD(getB());
                break;
            case 0x51:
                setD(getC());
                break;
            case 0x52:
                setD(getD());
                break;
            case 0x53:
                setD(getE());
                break;
            case 0x54:
                setD(getH());
                break;
            case 0x55:
                setD(getL());
                break;

            // destination E
            case 0x5F:
                setE(getA());
                break;
            case 0x58:
                setE(getB());
                break;
            case 0x59:
                setE(getC());
                break;
            case 0x5A:
                setE(getD());
                break;
            case 0x5B:
                setE(getE());
                break;
            case 0x5C:
                setE(getH());
                break;
            case 0x5D:
                setE(getL());
                break;

            // destination H
            case 0x67:
                setH(getA());
                break;
            case 0x60:
                setH(getB());
                break;
            case 0x61:
                setH(getC());
                break;
            case 0x62:
                setH(getD());
                break;
            case 0x63:
                setH(getE());
                break;
            case 0x64:
                setH(getH());
                break;
            case 0x65:
                setH(getL());
                break;

            // destination L
            case 0x6F:
                setL(getA());
                break;
            case 0x68:
                setL(getB());
                break;
            case 0x69:
                setL(getC());
                break;
            case 0x6A:
                setL(getD());
                break;
            case 0x6B:
                setL(getE());
                break;
            case 0x6C:
                setL(getH());
                break;
            case 0x6D:
                setL(getL());
                break;


            /*
             * LD r, n
             * Load 8-bit immediate n into register r
             */
            case 0x3E:
                setA(mem[PC + 1]); PC++;
                break;
            case 0x06:
                setB(mem[PC + 1]); PC++;
                break;
            case 0x0E:
                setC(mem[PC + 1]); PC++;
                break;
            case 0x16:
                setD(mem[PC + 1]); PC++;
                break;
            case 0x1E:
                setE(mem[PC + 1]); PC++;
                break;
            case 0x26:
                setH(mem[PC + 1]); PC++;
                break;
            case 0x2E:
                setL(mem[PC + 1]); PC++;
                break;

            /*
             * LD r, (HL)
             * Load memory at address HL into register r
             */
            case 0x7E:
                setA(mem[getHL()]);
                break;
            case 0x46:
                setB(mem[getHL()]);
                break;
            case 0x4E:
                setC(mem[getHL()]);
                break;
            case 0x56:
                setD(mem[getHL()]);
                break;
            case 0x5E:
                setE(mem[getHL()]);
                break;
            case 0x66:
                setH(mem[getHL()]);
                break;
            case 0x6E:
                setL(mem[getHL()]);
                break;

            /*
             * LD (HL), r
             * Load register r into memory at address HL
             */
            case 0x77:
                mem[getHL()] = getA();
                break;
            case 0x70:
                mem[getHL()] = getB();
                break;
            case 0x71:
                mem[getHL()] = getC();
                break;
            case 0x72:
                mem[getHL()] = getD();
                break;
            case 0x73:
                mem[getHL()] = getE();
                break;
            case 0x74:
                mem[getHL()] = getH();
                break;
            case 0x75:
                mem[getHL()] = getL();
                break;

            /*
             * LD (HL), n
             * Load 8-bit immediate n into memory at address HL
             */
            case 0x36:
                mem[getHL()] = mem[PC + 1]; PC++;
                break;

            /*
             * LD A, (BC)
             * Load memory at address BC into register A
             */
            case 0x0A:
                setA(mem[getBC()]);
                break;

            /*
             * LD A, (DE)
             * Load memory at address DE into register A
             */
            case 0x1A:
                setA(mem[getDE()]);
                break;

            /*
             * LD A, (C)
             * Load memory at address 0xFF00 + [register C] into register A
             */
            case 0xF2:
                setA(mem[0xFF00 + getC()]);
                break;

            /*
             * LD (C), A
             * Load register A into memory at address 0xFF00 + [register C]
             */
            case 0xE2:
                mem[0xFF00 + getC()] = getA();
                break;

            /*
             * LD A, (n)
             * Load memory at address 0xFF00 + [8-bit immediate n] into register A
             */
            case 0xF0:
                setA(mem[0xFF00 + mem[PC + 1]]); PC++;
                break;

            /*
             * LD (n), A
             * Load register A into memory at address 0xFF00 + [8-bit immediate n]
             */
            case 0xE0:
                mem[0xFF00 + mem[PC + 1]] = getA(); PC++;
                break;

            /*
             * LD A, (nn)
             * Load memory at address specified by 16-bit immediate nn into register A
             */
            case 0xFA:
                setA(mem[(mem[PC + 2] << 8) | mem[PC + 1]]); PC += 2;
                break;

            /*
             * LD (nn), A
             * Load register A into memory at address specified by 16-bit immediate nn
             */
            case 0xEA:
                mem[(mem[PC + 2] << 8) | mem[PC + 1]] = getA(); PC += 2;
                break;

            /*
             * LD A, (HLI)
             * Load memory at address HL into register A, then increment HL
             */
            case 0x2A:
                setA(mem[getHL()]);
                setHL(getHL() + 1);
                break;

            /*
             * LD A, (HLD)
             * Load memory at address HL into register A, then decrement HL
             */
            case 0x3A:
                setA(mem[getHL()]);
                setHL(getHL() - 1);
                break;

            /*
             * LD (BC), A
             * Load register A into memory at address BC
             */
            case 0x02:
                mem[getBC()] = getA();
                break;

            /*
             * LD (DE), A
             * Load register A into memory at address DE
             */
            case 0x12:
                mem[getDE()] = getA();
                break;

            /*
             * LD (HLI), A
             * Load register A into memory at address HL, then increment HL
             */
            case 0x22:
                mem[getHL()] = getA();
                setHL(getHL() + 1);
                break;

            /*
             * LD (HLD), A
             * Load register A into memory at address HL, then decrement HL
             */
            case 0x32:
                mem[getHL()] = getA();
                setHL(getHL() - 1);
                break;



            // 16-Bit Transfer Instructions

            /*
             * LD dd, nn
             * Load 2 byte immediate nn into register pair dd
             */
            case 0x01:
                setBC((mem[PC + 2] << 8) | mem[PC + 1]);
                PC += 2;
                break;
            case 0x11:
                setDE((mem[PC + 2] << 8) | mem[PC + 1]);
                PC += 2;
                break;
            case 0x21:
                setHL((mem[PC + 2] << 8) | mem[PC + 1]);
                PC += 2;
                break;
            case 0x31:
                SP = (mem[PC + 2] << 8) | mem[PC + 1];
                PC += 2;
                break;

            /*
             * LD SP, HL
             * Load register HL into SP
             */
            case 0xF9:
                SP = getHL();
                break;

            /*
             * PUSH qq
             * Push register qq onto stack
             */
            case 0xC5:
                mem[SP - 1] = getB();
                mem[SP - 2] = getC();
                SP -= 2;
                break;
            case 0xD5:
                mem[SP - 1] = getD();
                mem[SP - 2] = getE();
                SP -= 2;
                break;
            case 0xE5:
                mem[SP - 1] = getH();
                mem[SP - 2] = getL();
                SP -= 2;
                break;
            case 0xF5:
                mem[SP - 1] = getA();
                mem[SP - 2] = getF();
                SP -= 2;
                break;

            /*
             * POP qq
             * Pop contents of stack into register pair qq
             */
            case 0xC1:
                setC(mem[SP]);
                setB(mem[SP + 1]);
                SP += 2;
                break;
            case 0xD1:
                setE(mem[SP]);
                setD(mem[SP + 1]);
                SP += 2;
                break;
            case 0xE1:
                setL(mem[SP]);
                setH(mem[SP + 1]);
                SP += 2;
                break;
            case 0xF1:
                setF(mem[SP]);
                setA(mem[SP + 1]);
                SP += 2;
                break;

            /*
             * LDHL SP, e
             * Add 8-bit immediate e to SP and store result in HL
             */
            case 0xF8:
                setHL(add16(getHL(), mem[PC + 1]));
                PC++;
                break;

            /*
             * LD (nn), SP
             * Load stack pointer into memory at address specified by 16-bit immediate nn
             */
            case 0x08:
                mem[(mem[PC + 2] << 8) | mem[PC + 1]] = SP;
                PC += 2;
                break;



            // 8-Bit Arithmetic and Logical Operation Instructions

            /*
             * ADD A, r
             * Add register r to register A and store result in A
             */
            case 0x87:
                setA(add8(getA(), getA()));
                break;
            case 0x80:
                setA(add8(getA(), getB()));
                break;
            case 0x81:
                setA(add8(getA(), getC()));
                break;
            case 0x82:
                setA(add8(getA(), getD()));
                break;
            case 0x83:
                setA(add8(getA(), getE()));
                break;
            case 0x84:
                setA(add8(getA(), getH()));
                break;
            case 0x85:
                setA(add8(getA(), getL()));
                break;

            /*
             * ADD A, n
             * Add 8-bit immediate n with register A and store result in A
             */
            case 0xC6:
                setA(add8(getA(), mem[PC + 1]));
                PC++;
                break;

            /*
             * ADD A, (HL)
             * Add memory at address HL to register A and store result in A
             */
            case 0x86:
                setA(add8(getA(), mem[getHL()]));
                break;

            /*
             * ADC A, r
             * Add A, register r, and carry flag together and store result in A
             */
            case 0x8F:
                setA(add8(getA(), getA() + getCarry()));
                break;
            case 0x88:
                setA(add8(getA(), getB() + getCarry()));
                break;
            case 0x89:
                setA(add8(getA(), getC() + getCarry()));
                break;
            case 0x8A:
                setA(add8(getA(), getD() + getCarry()));
                break;
            case 0x8B:
                setA(add8(getA(), getE() + getCarry()));
                break;
            case 0x8C:
                setA(add8(getA(), getH() + getCarry()));
                break;
            case 0x8D:
                setA(add8(getA(), getL() + getCarry()));
                break;

            /*
             * ADC A, n
             * Add A, 8-bit immediate n, and carry flag together and store result in A
             */
            case 0xCE:
                setA(add8(getA(), mem[PC + 1] + getCarry()));
                PC++;
                break;

            /*
             * ADC A, (HL)
             * Add A, memory at address HL, and carry flag together and store result in A
             */
            case 0x8E:
                setA(add8(getA(), mem[getHL()] + getCarry()));
                break;

            /*
             * SUB r
             * Subtract register r from register A and store result in A
             */
            case 0x97:
                setA(sub8(getA(), getA()));
                break;
            case 0x90:
                setA(sub8(getA(), getB()));
                break;
            case 0x91:
                setA(sub8(getA(), getC()));
                break;
            case 0x92:
                setA(sub8(getA(), getD()));
                break;
            case 0x93:
                setA(sub8(getA(), getE()));
                break;
            case 0x94:
                setA(sub8(getA(), getH()));
                break;
            case 0x95:
                setA(sub8(getA(), getL()));
                break;

            /*
             * SUB n
             * Subtract 8-bit immediate n from register A and store result in A
             */
            case 0xD6:
                setA(sub8(getA(), mem[PC + 1]));
                PC++;
                break;

            /*
             * SUB (HL)
             * Subtract memory at address HL from register A and store result in A
             */
            case 0x96:
                setA(sub8(getA(), mem[getHL()]));
                break;

            /*
             * SBC A, r
             * Subtract register r and carry flag from register A and store result in A
             */
            case 0x9F:
                setA(sub8(getA(), getA() + getCarry()));
                break;
            case 0x98:
                setA(sub8(getA(), getB() + getCarry()));
                break;
            case 0x99:
                setA(sub8(getA(), getC() + getCarry()));
                break;
            case 0x9A:
                setA(sub8(getA(), getD() + getCarry()));
                break;
            case 0x9B:
                setA(sub8(getA(), getE() + getCarry()));
                break;
            case 0x9C:
                setA(sub8(getA(), getH() + getCarry()));
                break;
            case 0x9D:
                setA(sub8(getA(), getL() + getCarry()));
                break;

            /*
             * SBC A, n
             * Subtract 8-bit immediate n and carry flag from register A and store result in A
             */
            case 0xDE:
                setA(sub8(getA(), mem[PC + 1] + getCarry()));
                PC++;
                break;

            /*
             * SBC A, (HL)
             * Subtract memory at address HL and carry flag from register A and store result in A
             */
            case 0x9E:
                setA(sub8(getA(), mem[getHL()] + getCarry()));
                break;

            /*
             * AND r
             * And register A and register r together and store result in A
             */
            case 0xA7:
                setA(and8(getA(), getA()));
                break;
            case 0xA0:
                setA(and8(getA(), getB()));
                break;
            case 0xA1:
                setA(and8(getA(), getC()));
                break;
            case 0xA2:
                setA(and8(getA(), getD()));
                break;
            case 0xA3:
                setA(and8(getA(), getE()));
                break;
            case 0xA4:
                setA(and8(getA(), getH()));
                break;
            case 0xA5:
                setA(and8(getA(), getL()));
                break;

            /*
             * AND n
             * And register A and 8-bit immediate n together and store result in A
             */
            case 0xE6:
                setA(and8(getA(), mem[PC + 1]));
                PC++;
                break;

            /*
             * AND (HL)
             * And register A and memory at address HL together and store result in A
             */
            case 0xA6:
                setA(and8(getA(), mem[getHL()]));
                break;

            /*
             * OR r
             * Or register A and register r together and store result in A
             */
            case 0xB7:
                setA(or8(getA(), getA()));
                break;
            case 0xB0:
                setA(or8(getA(), getB()));
                break;
            case 0xB1:
                setA(or8(getA(), getC()));
                break;
            case 0xB2:
                setA(or8(getA(), getD()));
                break;
            case 0xB3:
                setA(or8(getA(), getE()));
                break;
            case 0xB4:
                setA(or8(getA(), getH()));
                break;
            case 0xB5:
                setA(or8(getA(), getL()));
                break;

            /*
             * OR n
             * Or register A and 8-bit immediate n together and store result in A
             */
            case 0xF6:
                setA(or8(getA(), mem[PC + 1]));
                PC++;
                break;

            /*
             * OR (HL)
             * Or register A and memory at address HL together and store result in A
             */
            case 0xB6:
                setA(or8(getA(), mem[getHL()]));
                break;

            /*
             * XOR r
             * Xor register A and register r together and store result in A
             */
            case 0xAF:
                setA(xor8(getA(), getA()));
                break;
            case 0xA8:
                setA(xor8(getA(), getB()));
                break;
            case 0xA9:
                setA(xor8(getA(), getC()));
                break;
            case 0xAA:
                setA(xor8(getA(), getD()));
                break;
            case 0xAB:
                setA(xor8(getA(), getE()));
                break;
            case 0xAC:
                setA(xor8(getA(), getH()));
                break;
            case 0xAD:
                setA(xor8(getA(), getL()));
                break;

            /*
             * XOR n
             * Xor register A and 8-bit immediate n together and store result in A
             */
            case 0xEE:
                setA(xor8(getA(), mem[PC + 1]));
                PC++;
                break;

            /*
             * XOR (HL)
             * Xor register A and memory at address HL together and store result in A
             */
            case 0xAE:
                setA(xor8(getA(), mem[getHL()]));
                break;

            /*
             * CP r
             * Compare register A and register R
             */
            case 0xBF:
                sub8(getA(), getA());
                break;
            case 0xB8:
                sub8(getA(), getB());
                break;
            case 0xB9:
                sub8(getA(), getC());
                break;
            case 0xBA:
                sub8(getA(), getD());
                break;
            case 0xBB:
                sub8(getA(), getE());
                break;
            case 0xBC:
                sub8(getA(), getH());
                break;
            case 0xBD:
                sub8(getA(), getL());
                break;

            /*
             * CP n
             * Compare register A and 8-bit immediate n
             */
            case 0xFE:
                sub8(getA(), mem[PC + 1]);
                PC++;
                break;

            /*
             * CP (HL)
             * Compare register A and memory at address HL
             */
            case 0xBE:
                sub8(getA(), mem[getHL()]);
                break;

            /*
             * INC r
             * Increment register r
             */
            case 0x3C:
                setA(inc(getA()));
                break;
            case 0x04:
                setB(inc(getB()));
                break;
            case 0x0C:
                setC(inc(getC()));
                break;
            case 0x14:
                setD(inc(getD()));
                break;
            case 0x1C:
                setE(inc(getE()));
                break;
            case 0x24:
                setH(inc(getH()));
                break;
            case 0x2C:
                setL(inc(getL()));
                break;

            /*
             * INC (HL)
             * Increment memory at address HL
             */
            case 0x34:
                mem[getHL()] = inc(mem[getHL()]);
                break;

            /*
             * DEC r
             * Decrement register r
             */
            case 0x3D:
                setA(dec(getA()));
                break;
            case 0x05:
                setB(dec(getB()));
                break;
            case 0x0D:
                setC(dec(getC()));
                break;
            case 0x15:
                setD(dec(getD()));
                break;
            case 0x1D:
                setE(dec(getE()));
                break;
            case 0x25:
                setH(dec(getH()));
                break;
            case 0x2D:
                setL(dec(getL()));
                break;

            /*
             * DEC (HL)
             * Decrement memory at address HL
             */
            case 0x35:
                mem[getHL()] = dec(mem[getHL()]);
                break;


            // 16-Bit Arithmetic Operation Instructions

            /*
             * ADD HL, ss
             * Add register HL and register ss and store store result in HL
             */
            case 0x09:
                setHL(add16(getHL(), getBC()));
                break;
            case 0x19:
                setHL(add16(getHL(), getDE()));
                break;
            case 0x29:
                setHL(add16(getHL(), getHL()));
                break;
            case 0x39:
                setHL(add16(getHL(), SP));
                break;

            /*
             * ADD SP, e
             * Add 8-bit immediate e to SP and store result in SP
             */
            case 0xE8:
                SP = add16(SP, mem[PC + 1]);
                PC++;
                break;

            /*
             * INC ss
             * Increment register ss
             */
            case 0x03:
                setBC(getBC() + 1);
                break;
            case 0x13:
                setDE(getDE() + 1);
                break;
            case 0x23:
                setHL(getHL() + 1);
                break;
            case 0x33:
                SP++;
                break;

            /*
             * DEC ss
             * Decrement register ss
             */
            case 0x0B:
                setBC(getBC() - 1);
                break;
            case 0x1B:
                setDE(getDE() - 1);
                break;
            case 0x2B:
                setHL(getHL() - 1);
                break;
            case 0x3B:
                SP--;
                break;



            // Rotate Shift Instructions

            /*
             * RLCA
             * Rotate register A to the left
             */
            case 0x07:
                setA(rotateLeft(getA()));
                break;

            /*
             * RLA
             * Rotate register A to the left through the carry flag
             */
            case 0x17:
                setA(rotateLeftCarry(getA()));
                break;

            /*
             * RRCA
             * Rotate register A to the right
             */
            case 0x0F:
                setA(rotateRight(getA()));
                break;

            /*
             * RRA
             * Rotate register A to the right through the carry flag
             */
            case 0x1F:
                setA(rotateRightCarry(getA()));
                break;

            // multi-function opcode
            case 0xCB:
                switch(mem[PC + 1]) {

                    /*
                     * RLC r
                     * Rotate register r to the left
                     */
                    case 0x07:
                        setA(rotateLeft(getA()));
                        break;
                    case 0x00:
                        setB(rotateLeft(getB()));
                        break;
                    case 0x01:
                        setC(rotateLeft(getC()));
                        break;
                    case 0x02:
                        setD(rotateLeft(getD()));
                        break;
                    case 0x03:
                        setE(rotateLeft(getE()));
                        break;
                    case 0x04:
                        setH(rotateLeft(getH()));
                        break;
                    case 0x05:
                        setL(rotateLeft(getL()));
                        break;

                    /*
                     * RLC (HL)
                     * Rotate memory at address HL to the left
                     */
                    case 0x06:
                        mem[getHL()] = rotateLeft(mem[getHL()]);
                        break;

                    /*
                     * RL r
                     * Rotate register r to the left through carry flag
                     */
                    case 0x17:
                        setA(rotateLeftCarry(getA()));
                        break;
                    case 0x10:
                        setB(rotateLeftCarry(getB()));
                        break;
                    case 0x11:
                        setC(rotateLeftCarry(getC()));
                        break;
                    case 0x12:
                        setD(rotateLeftCarry(getD()));
                        break;
                    case 0x13:
                        setE(rotateLeftCarry(getE()));
                        break;
                    case 0x14:
                        setH(rotateLeftCarry(getH()));
                        break;
                    case 0x15:
                        setL(rotateLeftCarry(getL()));
                        break;

                    /*
                     * RL (HL)
                     * Rotate memory at address HL to the left through carry flag
                     */
                    case 0x16:
                        mem[getHL()] = rotateLeftCarry(mem[getHL()]);
                        break;

                    /*
                     * RRC r
                     * Rotate register r to the right
                     */
                    case 0x0F:
                        setA(rotateRight(getA()));
                        break;
                    case 0x08:
                        setB(rotateRight(getB()));
                        break;
                    case 0x09:
                        setC(rotateRight(getC()));
                        break;
                    case 0x0A:
                        setD(rotateRight(getD()));
                        break;
                    case 0x0B:
                        setE(rotateRight(getE()));
                        break;
                    case 0x0C:
                        setH(rotateRight(getH()));
                        break;
                    case 0x0D:
                        setL(rotateRight(getL()));
                        break;

                    /*
                     * RRC (HL)
                     * Rotate memory at address HL to the right
                     */
                    case 0x0E:
                        mem[getHL()] = rotateLeft(mem[getHL()]);
                        break;

                    /*
                     * RR r
                     * Rotate register r to the right through carry flag
                     */
                    case 0x1F:
                        setA(rotateRightCarry(getA()));
                        break;
                    case 0x18:
                        setB(rotateRightCarry(getB()));
                        break;
                    case 0x19:
                        setC(rotateRightCarry(getC()));
                        break;
                    case 0x1A:
                        setD(rotateRightCarry(getD()));
                        break;
                    case 0x1B:
                        setE(rotateRightCarry(getE()));
                        break;
                    case 0x1C:
                        setH(rotateRightCarry(getH()));
                        break;
                    case 0x1D:
                        setL(rotateRightCarry(getL()));
                        break;

                    /*
                     * RR (HL)
                     * Rotate memory at address HL to the left through carry flag
                     */
                    case 0x1E:
                        mem[getHL()] = rotateRightCarry(mem[getHL()]);
                        break;

                    /*
                     * SLA r
                     * Shift register r to the left
                     */
                    case 0x27:
                        setA(shiftLeft(getA()));
                        break;
                    case 0x20:
                        setB(shiftLeft(getB()));
                        break;
                    case 0x21:
                        setC(shiftLeft(getC()));
                        break;
                    case 0x22:
                        setD(shiftLeft(getD()));
                        break;
                    case 0x23:
                        setE(shiftLeft(getE()));
                        break;
                    case 0x24:
                        setH(shiftLeft(getH()));
                        break;
                    case 0x25:
                        setL(shiftLeft(getL()));
                        break;

                    /*
                     * SLA (HL)
                     * Shift memory at register HL left
                     */
                    case 0x26:
                        mem[getHL()] = shiftLeft(mem[getHL()]);
                        break;

                    /*
                     * SRA r
                     * Shift register r to the right
                     */
                    case 0x2F:
                        setA(shiftRightArithmetic(getA()));
                        break;
                    case 0x28:
                        setB(shiftRightArithmetic(getB()));
                        break;
                    case 0x29:
                        setC(shiftRightArithmetic(getC()));
                        break;
                    case 0x2A:
                        setD(shiftRightArithmetic(getD()));
                        break;
                    case 0x2B:
                        setE(shiftRightArithmetic(getE()));
                        break;
                    case 0x2C:
                        setH(shiftRightArithmetic(getH()));
                        break;
                    case 0x2D:
                        setL(shiftRightArithmetic(getL()));
                        break;

                    /*
                     * SRA (HL)
                     * Shift memory at HL to the right
                     */
                    case 0x2E:
                        mem[getHL()] = shiftRightArithmetic(mem[getHL()]);
                        break;

                    /*
                     * SRL r
                     * Shift register r to the right logically
                     */
                    case 0x3F:
                        setA(shiftRightLogical(getA()));
                        break;
                    case 0x38:
                        setB(shiftRightLogical(getB()));
                        break;
                    case 0x39:
                        setC(shiftRightLogical(getC()));
                        break;
                    case 0x3A:
                        setD(shiftRightLogical(getD()));
                        break;
                    case 0x3B:
                        setE(shiftRightLogical(getE()));
                        break;
                    case 0x3C:
                        setH(shiftRightLogical(getH()));
                        break;
                    case 0x3D:
                        setL(shiftRightLogical(getL()));
                        break;

                    /*
                     * SRL (HL)
                     * Shift memory at HL to the right logically
                     */
                    case 0x3E:
                        mem[getHL()] = shiftRightLogical(mem[getHL()]);
                        break;

                    /*
                     * SWAP r
                     * Swap upper and lower nibbles of register r
                     */
                    case 0x37:
                        setA(swap(getA()));
                        break;
                    case 0x30:
                        setB(swap(getB()));
                        break;
                    case 0x31:
                        setC(swap(getC()));
                        break;
                    case 0x32:
                        setD(swap(getD()));
                        break;
                    case 0x33:
                        setE(swap(getE()));
                        break;
                    case 0x34:
                        setH(swap(getH()));
                        break;
                    case 0x35:
                        setL(swap(getL()));
                        break;

                    /*
                     * SWAP (HL)
                     * Swap upper and lower nibbles of memory at HL
                     */
                    case 0x36:
                        mem[getHL()] = swap(mem[getHL()]);
                        break;

                    /*
                     * BIT b, r
                     * Copies complement of specified bit b of register r to Z flag
                     */

                    // register A
                    case 0x47:
                        bit(getA(), 0);
                        break;
                    case 0x4F:
                        bit(getA(), 1);
                        break;
                    case 0x57:
                        bit(getA(), 2);
                        break;
                    case 0x5F:
                        bit(getA(), 3);
                        break;
                    case 0x67:
                        bit(getA(), 4);
                        break;
                    case 0x6F:
                        bit(getA(), 5);
                        break;
                    case 0x77:
                        bit(getA(), 6);
                        break;
                    case 0x7F:
                        bit(getA(), 7);
                        break;

                    // register B
                    case 0x40:
                        bit(getB(), 0);
                        break;
                    case 0x48:
                        bit(getB(), 1);
                        break;
                    case 0x50:
                        bit(getB(), 2);
                        break;
                    case 0x58:
                        bit(getB(), 3);
                        break;
                    case 0x60:
                        bit(getB(), 4);
                        break;
                    case 0x68:
                        bit(getB(), 5);
                        break;
                    case 0x70:
                        bit(getB(), 6);
                        break;
                    case 0x78:
                        bit(getB(), 7);
                        break;

                    // register C
                    case 0x41:
                        bit(getC(), 0);
                        break;
                    case 0x49:
                        bit(getC(), 1);
                        break;
                    case 0x51:
                        bit(getC(), 2);
                        break;
                    case 0x59:
                        bit(getC(), 3);
                        break;
                    case 0x61:
                        bit(getC(), 4);
                        break;
                    case 0x69:
                        bit(getC(), 5);
                        break;
                    case 0x71:
                        bit(getC(), 6);
                        break;
                    case 0x79:
                        bit(getC(), 7);
                        break;

                    // register D
                    case 0x42:
                        bit(getD(), 0);
                        break;
                    case 0x4A:
                        bit(getD(), 1);
                        break;
                    case 0x52:
                        bit(getD(), 2);
                        break;
                    case 0x5A:
                        bit(getD(), 3);
                        break;
                    case 0x62:
                        bit(getD(), 4);
                        break;
                    case 0x6A:
                        bit(getD(), 5);
                        break;
                    case 0x72:
                        bit(getD(), 6);
                        break;
                    case 0x7A:
                        bit(getD(), 7);
                        break;

                    // register E
                    case 0x43:
                        bit(getE(), 0);
                        break;
                    case 0x4B:
                        bit(getE(), 1);
                        break;
                    case 0x53:
                        bit(getE(), 2);
                        break;
                    case 0x5B:
                        bit(getE(), 3);
                        break;
                    case 0x63:
                        bit(getE(), 4);
                        break;
                    case 0x6B:
                        bit(getE(), 5);
                        break;
                    case 0x73:
                        bit(getE(), 6);
                        break;
                    case 0x7B:
                        bit(getE(), 7);
                        break;

                    // register H
                    case 0x44:
                        bit(getH(), 0);
                        break;
                    case 0x4C:
                        bit(getH(), 1);
                        break;
                    case 0x54:
                        bit(getH(), 2);
                        break;
                    case 0x5C:
                        bit(getH(), 3);
                        break;
                    case 0x64:
                        bit(getH(), 4);
                        break;
                    case 0x6C:
                        bit(getH(), 5);
                        break;
                    case 0x74:
                        bit(getH(), 6);
                        break;
                    case 0x7C:
                        bit(getH(), 7);
                        break;

                    // register L
                    case 0x45:
                        bit(getL(), 0);
                        break;
                    case 0x4D:
                        bit(getL(), 1);
                        break;
                    case 0x55:
                        bit(getL(), 2);
                        break;
                    case 0x5D:
                        bit(getL(), 3);
                        break;
                    case 0x65:
                        bit(getL(), 4);
                        break;
                    case 0x6D:
                        bit(getL(), 5);
                        break;
                    case 0x75:
                        bit(getL(), 6);
                        break;
                    case 0x7D:
                        bit(getL(), 7);
                        break;

                    /*
                     * BIT b, (HL)
                     * Copies complement of specified bit b of memory at HL to Z flag
                     */
                    case 0x46:
                        bit(mem[getHL()], 0);
                        break;
                    case 0x4E:
                        bit(mem[getHL()], 1);
                        break;
                    case 0x56:
                        bit(mem[getHL()], 2);
                        break;
                    case 0x5E:
                        bit(mem[getHL()], 3);
                        break;
                    case 0x66:
                        bit(mem[getHL()], 4);
                        break;
                    case 0x6E:
                        bit(mem[getHL()], 5);
                        break;
                    case 0x76:
                        bit(mem[getHL()], 6);
                        break;
                    case 0x7E:
                        bit(mem[getHL()], 7);
                        break;

                    /*
                     * SET b, r
                     * Sets bit b in register r to 1
                     */
                    
                    // register A
                    case 0xC7:
                        setA(set(getA(), 0));
                        break;
                    case 0xCF:
                        setA(set(getA(), 1));
                        break;
                    case 0xD7:
                        setA(set(getA(), 2));
                        break;
                    case 0xDF:
                        setA(set(getA(), 3));
                        break;
                    case 0xE7:
                        setA(set(getA(), 4));
                        break;
                    case 0xEF:
                        setA(set(getA(), 5));
                        break;
                    case 0xF7:
                        setA(set(getA(), 6));
                        break;
                    case 0xFF:
                        setA(set(getA(), 7));
                        break;
                        
                    // register B
                    case 0xC0:
                        setB(set(getB(), 0));
                        break;
                    case 0xC8:
                        setB(set(getB(), 1));
                        break;
                    case 0xD0:
                        setB(set(getB(), 2));
                        break;
                    case 0xD8:
                        setB(set(getB(), 3));
                        break;
                    case 0xE0:
                        setB(set(getB(), 4));
                        break;
                    case 0xE8:
                        setB(set(getB(), 5));
                        break;
                    case 0xF0:
                        setB(set(getB(), 6));
                        break;
                    case 0xF8:
                        setB(set(getB(), 7));
                        break;
                        
                    // register C
                    case 0xC1:
                        setC(set(getC(), 0));
                        break;
                    case 0xC9:
                        setC(set(getC(), 1));
                        break;
                    case 0xD1:
                        setC(set(getC(), 2));
                        break;
                    case 0xD9:
                        setC(set(getC(), 3));
                        break;
                    case 0xE1:
                        setC(set(getC(), 4));
                        break;
                    case 0xE9:
                        setC(set(getC(), 5));
                        break;
                    case 0xF1:
                        setC(set(getC(), 6));
                        break;
                    case 0xF9:
                        setC(set(getC(), 7));
                        break;
                        
                    // register D
                    case 0xC2:
                        setD(set(getD(), 0));
                        break;
                    case 0xCA:
                        setD(set(getD(), 1));
                        break;
                    case 0xD2:
                        setD(set(getD(), 2));
                        break;
                    case 0xDA:
                        setD(set(getD(), 3));
                        break;
                    case 0xE2:
                        setD(set(getD(), 4));
                        break;
                    case 0xEA:
                        setD(set(getD(), 5));
                        break;
                    case 0xF2:
                        setD(set(getD(), 6));
                        break;
                    case 0xFA:
                        setD(set(getD(), 7));
                        break;

                    // register E
                    case 0xC3:
                        setE(set(getE(), 0));
                        break;
                    case 0xCB:
                        setE(set(getE(), 1));
                        break;
                    case 0xD3:
                        setE(set(getE(), 2));
                        break;
                    case 0xDB:
                        setE(set(getE(), 3));
                        break;
                    case 0xE3:
                        setE(set(getE(), 4));
                        break;
                    case 0xEB:
                        setE(set(getE(), 5));
                        break;
                    case 0xF3:
                        setE(set(getE(), 6));
                        break;
                    case 0xFB:
                        setE(set(getE(), 7));
                        break;
                        
                    // register H
                    case 0xC4:
                        setH(set(getH(), 0));
                        break;
                    case 0xCC:
                        setH(set(getH(), 1));
                        break;
                    case 0xD4:
                        setH(set(getH(), 2));
                        break;
                    case 0xDC:
                        setH(set(getH(), 3));
                        break;
                    case 0xE4:
                        setH(set(getH(), 4));
                        break;
                    case 0xEC:
                        setH(set(getH(), 5));
                        break;
                    case 0xF4:
                        setH(set(getH(), 6));
                        break;
                    case 0xFC:
                        setH(set(getH(), 7));
                        break;

                    // register L
                    case 0xC5:
                        setL(set(getL(), 0));
                        break;
                    case 0xCD:
                        setL(set(getL(), 1));
                        break;
                    case 0xD5:
                        setL(set(getL(), 2));
                        break;
                    case 0xDD:
                        setL(set(getL(), 3));
                        break;
                    case 0xE5:
                        setL(set(getL(), 4));
                        break;
                    case 0xED:
                        setL(set(getL(), 5));
                        break;
                    case 0xF5:
                        setL(set(getL(), 6));
                        break;
                    case 0xFD:
                        setL(set(getL(), 7));
                        break;

                    /*
                     * SET b, (HL)
                     * Sets bit b in memory at HL to 1
                     */
                    case 0xC6:
                        mem[getHL()] = set(mem[getHL()], 0);
                        break;
                    case 0xCE:
                        mem[getHL()] = set(mem[getHL()], 1);
                        break;
                    case 0xD6:
                        mem[getHL()] = set(mem[getHL()], 2);
                        break;
                    case 0xDE:
                        mem[getHL()] = set(mem[getHL()], 3);
                        break;
                    case 0xE6:
                        mem[getHL()] = set(mem[getHL()], 4);
                        break;
                    case 0xEE:
                        mem[getHL()] = set(mem[getHL()], 5);
                        break;
                    case 0xF6:
                        mem[getHL()] = set(mem[getHL()], 6);
                        break;
                    case 0xFE:
                        mem[getHL()] = set(mem[getHL()], 7);
                        break;

                    /*
                     * RES b, r
                     * Set bit b in register r to 0
                     */
                }
                break;
        }
    }

    uint8_t add8(uint8_t a, uint8_t b) {
        uint16_t sum = a + b;
        uint8_t half = (a & 0xF) + (b & 0xF);
        setZero(sum == 0);
        setHalfCarry(half > 0xF);
        setSubtract(false);
        setCarry(sum > 0xFF);
        return (uint8_t) sum;
    }

    uint16_t add16(uint16_t a, uint16_t b) {
        uint32_t sum = a + b;
        uint16_t half = (a & 0xFF) + (b & 0xFF);
        setZero(sum == 0);
        setHalfCarry(half > 0xFF);
        setSubtract(false);
        setCarry(sum > 0xFFFF);
        return (uint16_t) sum;
    }

    uint8_t sub8(uint8_t a, uint8_t b) {
        uint8_t diff = a - b;
        setZero(diff == 0);
        setHalfCarry((a & 0xF) < (b & 0xF));
        setSubtract(true);
        setCarry(a < b);
        return diff;
    }

    uint8_t and8(uint8_t a, uint8_t b) {
        uint8_t result = a & b;
        setZero(result == 0);
        setHalfCarry(true);
        setSubtract(false);
        setCarry(false);
        return result;
    }

    uint8_t or8(uint8_t a, uint8_t b) {
        uint8_t result = a | b;
        setZero(result == 0);
        return result;
    }

    uint8_t xor8(uint8_t a, uint8_t b) {
        uint8_t result = a ^ b;
        setZero(result == 0);
        return result;
    }

    uint8_t inc(uint8_t value) {
        bool carry = getCarry();
        uint8_t sum = add8(value, 1);
        setCarry(carry);
        return sum;
    }

    uint8_t dec(uint8_t value) {
        bool carry = getCarry();
        uint8_t diff = sub8(value, 1);
        setCarry(carry);
        return diff;
    }

    uint8_t rotateLeft(uint8_t value) {
        uint8_t bit7 = (value & 0x80) >> 7;
        uint8_t result = (value << 1) | bit7;
        setCarry(bit7);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return (value << 1) | bit7;
    }

    uint8_t rotateLeftCarry(uint8_t value) {
        uint8_t bit7 = (value & 0x80) >> 7;
        auto carry = (uint8_t)getCarry();
        uint8_t result = (value << 1) | carry;
        setCarry(bit7);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t rotateRight(uint8_t value) {
        uint8_t bit0 = value & 0x1;
        uint8_t result = (value >> 1) | (bit0 << 7);
        setCarry(bit0);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t rotateRightCarry(uint8_t value) {
        uint8_t bit0 = value & 0x1;
        auto carry = (uint8_t)getCarry();
        uint8_t result = (value >> 1) | (carry << 7);
        setCarry(bit0);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t shiftLeft(uint8_t value) {
        uint8_t result = value << 1;
        setCarry((value & 0x80) >> 7);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t shiftRightArithmetic(uint8_t value) {
        uint8_t msb = value & 0x80;
        uint8_t result = (value >> 1) | msb;
        setCarry(value & 0x1);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t shiftRightLogical(uint8_t value) {
        uint8_t result = (value >> 1) & 0x7F;
        setCarry(value & 0x1);
        setZero(result == 0);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    uint8_t swap(uint8_t value) {
        uint8_t hi = (value >> 4) & 0xF;
        uint8_t lo = (value << 4) & 0xF0;
        uint8_t result = lo | hi;
        setZero(result == 0);
        setCarry(false);
        setHalfCarry(false);
        setSubtract(false);
        return result;
    }

    void bit(uint8_t value, int bit) {
        setZero(~(value >> bit) & 0x1);
    }

    uint8_t set(uint8_t value, int bit) {
        return value | (1 << bit);
    }

    uint8_t reset(uint8_t value, int bit) {
        return value & ~(1 << bit);
    }

    // set 8 bit registers
    void setA(uint8_t value) { AF = (AF & 0xFF) | (value << 8); }
    void setF(uint8_t value) { AF = (AF & 0xFF00) | value; }
    void setB(uint8_t value) { BC = (BC & 0xFF) | (value << 8); }
    void setC(uint8_t value) { BC = (BC & 0xFF00) | value; }
    void setD(uint8_t value) { DE = (DE & 0xFF) | (value << 8); }
    void setE(uint8_t value) { DE = (DE & 0xFF00) | value; }
    void setH(uint8_t value) { HL = (HL & 0xFF) | (value << 8); }
    void setL(uint8_t value) { HL = (HL & 0xFF00) | value; }

    // get 8 bit registers
    uint8_t getA() { return (AF >> 8) & 0xFF; }
    uint8_t getF() { return AF & 0xFF; }
    uint8_t getB() { return (BC >> 8) & 0xFF; }
    uint8_t getC() { return BC & 0xFF; }
    uint8_t getD() { return (DE >> 8) & 0xFF; }
    uint8_t getE() { return DE & 0xFF; }
    uint8_t getH() { return (HL >> 8) & 0xFF; }
    uint8_t getL() { return HL & 0xFF; }

    // set 16 bit registers
    void setAF(uint16_t value) { AF = value; }
    void setBC(uint16_t value) { BC = value; }
    void setDE(uint16_t value) { DE = value; }
    void setHL(uint16_t value) { HL = value; }

    // get 16 bit registers
    uint16_t getAF() { return AF; }
    uint16_t getBC() { return BC; }
    uint16_t getDE() { return DE; }
    uint16_t getHL() { return HL; }

    // set flags
    void setZero(bool value) { AF = (AF & 0xFF7F) | ((uint8_t)value << 7); }
    void setSubtract(bool value) { AF = (AF & 0xFFBF) | ((uint8_t)value << 6); }
    void setHalfCarry(bool value) { AF = (AF & 0xFFDF) | ((uint8_t)value << 5); }
    void setCarry(bool value) { AF = (AF & 0xFFEF) | ((uint8_t)value << 4); }

    // get flags
    bool getZero() { return (AF >> 7) & 0x1; }
    bool getSubtract() { return (AF >> 6) & 0x1; }
    bool getHalfCarry() { return (AF >> 5) & 0x1; }
    bool getCarry() { return (AF >> 4) & 0x1; }

};