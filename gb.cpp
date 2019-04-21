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
                uint8_t carry = getCarry();
                setA(add8(getA(), getA()));
                setA(add8(getA(), carry));
                break;
            case 0x80:
                uint8_t carry = getCarry();
                setA(add8(getA(), getA()));
                setA(add8(getA(), carry));
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