class GameBoy {
private:
    unsigned char A, F; // 8 bit registers
    unsigned short BC, DE, HL, PC, SP; // 16 bit registers
    unsigned char *ram; // memory
    bool interrupts;

public:
    GameBoy() {
        A = 0x0;
        F = 0x0;
        BC = 0x0;
        DE = 0x0;
        HL = 0x0;
        PC = 0x0;
        SP = 0x0;
        ram = new unsigned char[0x2000];
        interrupts = true;
    }

    void decode() {

        unsigned char n8 = ram[PC + 1];
        unsigned short n16 = ram[PC + 2] << 8 | n8;

        switch(ram[PC]) {

            /* <><><><><><><><><><> 8-Bit Loads <><><><><><><><><><> */

            /*
             * LD nn, n
             * Put immediate value nn into n
             */
            case 0x06:
                setB(n8);
                break;
            case 0x0E:
                setC(n8);
                break;
            case 0x16:
                setD(n8);
                break;
            case 0x1E:
                setE(n8);
                break;
            case 0x26:
                setH(n8);
                break;
            case 0x2E:
                setL(n8);
                break;


            /*
             * LD r1, r2
             * Put value of register r1 into register r2
             */
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
            case 0x7E:
                setA(ram[getHL()]);
                break;
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
            case 0x46:
                setB(ram[getHL()]);
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
            case 0x4E:
                setC(ram[getHL()]);
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
            case 0x56:
                setD(ram[getHL()]);
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
            case 0x5E:
                setE(ram[getHL()]);
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
            case 0x66:
                setH(ram[getHL()]);
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
            case 0x6E:
                setL(ram[getHL()]);
                break;
            case 0x70:
                ram[getHL()] = getB();
                break;
            case 0x71:
                ram[getHL()] = getC();
                break;
            case 0x72:
                ram[getHL()] = getD();
                break;
            case 0x73:
                ram[getHL()] = getE();
                break;
            case 0x74:
                ram[getHL()] = getH();
                break;
            case 0x75:
                ram[getHL()] = getL();
                break;
            case 0x36:
                ram[getHL()] = n8;
                break;

            /*
             * LD A,n
             * Put value of register n into register A
             */
            case 0x0A:
                setA(ram[getBC()]);
                break;
            case 0x1A:
                setA(ram[getDE()]);
                break;
            case 0xFA:
                setA(ram[n16]);
                break;
            case 0x3E: // TBI
                break;

            /*
             * LD n,A
             * Put value of register A into register n
             */
            case 0x47:
                setB(getA());
                break;
            case 0x4F:
                setC(getA());
                break;
            case 0x57:
                setD(getA());
                break;
            case 0x5F:
                setE(getA());
                break;
            case 0x67:
                setH(getA());
                break;
            case 0x6F:
                setL(getA());
                break;
            case 0x02:
                ram[getBC()] = getA();
                break;
            case 0x12:
                ram[getDE()] = getA();
                break;
            case 0x77:
                ram[getHL()] = getA();
                break;
            case 0xEA:
                ram[n16] = getA();
                break;

            /*
             * LD A,(C)
             * Put value at address $FF00 + register C into A
             */
            case 0xF2:
                setA(ram[0xFF00 + static_cast<signed char>(getC())]);
                break;

            /*
             * LD (C),A
             * Put value in register A into address $FF00 + register C
             */
            case 0xE2:
                ram[0xFF00 + static_cast<signed char>(getC())] = getA();
                break;

            /*
             * LD A,(HLD)
             * Put value at address HL into A. Decrement HL.
             */
            case 0x3A:
                setA(ram[getHL()]);
                decHL();
                break;

            /*
             * LD A,(HLI)
             * Put value at address HL into A. Increment HL.
             */
            case 0x2A:
                setA(ram[getHL()]);
                incHL();
                break;

            /*
             * LD (HLI),A
             * Put A into memory address HL. Increment HL.
             */
            case 0x22:
                ram[getHL()] = getA();
                incHL();
                break;

            /*
             * LDH (n),A
             * Put value of A into memory address $FF00 + n
             */
            case 0xE0:
                ram[0xFF00 + n8] = getA();
                break;

            /*
             * LDH A,(n)
             * Put memory address 0xFF00+n into A
             */
            case 0xF0:
                setA(ram[0xFF00 + n8]);
                break;

            /* <><><><><><><><><><> 16-Bit Loads <><><><><><><><><><> */

            /*
             * LD n,nn
             * Put value nn into n
             */
            case 0x01:
                setBC(n16);
                break;
            case 0x11:
                setDE(n16);
                break;
            case 0x21:
                setHL(n16);
                break;
            case 0x31:
                SP = n16;
                break;

            /*
             * Put HL into stack pointer
             */
            case 0xF9:
                SP = getHL();
                break;

            /*
             * LDHL SP,n
             * Put SP + n effective address into HL
             */
            case 0xF8:
                setHL(SP + n8);
                break;

            /*
             * LD (nn),SP
             * Put stack pointer at address n
             */
            case 0x08: // MAYBE WRONG
                ram[n16] = static_cast<unsigned char>(SP & 0xFF);
                ram[n16 + 1] = static_cast<unsigned char>((SP >> 8) & 0xFF);
                break;

            /*
             * PUSH nn
             * Push register pair nn onto stack
             * Decrement SP twice
             */
            case 0xF5:
                ram[SP] = getF();
                ram[SP - 1] = getA();
                SP -= 2;
                break;
            case 0xC5:
                ram[SP] = getC();
                ram[SP - 1] = getB();
                SP -= 2;
                break;
            case 0xD5:
                ram[SP] = getE();
                ram[SP - 1] = getD();
                SP -= 2;
                break;
            case 0xE5:
                ram[SP] = getL();
                ram[SP - 1] = getH();
                SP -= 2;
                break;

            /*
             * POP nn
             * Pop two bytes off stack into register pair nn
             */
            case 0xF1:
                SP += 2;
                ram[SP] = getF();
                ram[SP + 1] = getA();
                break;
            case 0xC1:
                SP += 2;
                ram[SP] = getC();
                ram[SP + 1] = getB();
                break;
            case 0xD1:
                SP += 2;
                ram[SP] = getE();
                ram[SP + 1] = getD();
                break;
            case 0xE1:
                SP += 2;
                ram[SP] = getL();
                ram[SP + 1] = getH();
                break;

            /* <><><><><><><><><><> 8-Bit ALU <><><><><><><><><><> */

            /*
             * ADD A,n
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
            case 0x86:
                setA(add8(getA(), ram[getHL()]));
                break;
            case 0xC6:
                setA(add8(getA(), n8));
                break;

            /*
             * ADC A,n
             * Add n + carry flag to A
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
            case 0x8E:
                setA(add8(getA(), ram[getHL()] + getCarry()));
                break;
            case 0xCE:
                setA(add8(getA(), n8 + getCarry()));
                break;

            /*
             * SUB n
             * Subtract n from A
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
            case 0x96:
                setA(sub8(getA(), ram[getHL()]));
                break;
            case 0xD6:
                setA(sub8(getA(), n8));
                break;

            /*
             * SBC A,n
             * Subtract n + carry flag from A
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
            case 0x9E:
                setA(sub8(getA(), ram[getHL()] + getCarry()));
                break;

            /*
             * AND n
             * logically AND n with A, put result in A
             */
            case 0xA7:
                and8(getA());
                break;
            case 0xA0:
                and8(getB());
                break;
            case 0xA1:
                and8(getC());
                break;
            case 0xA2:
                and8(getD());
                break;
            case 0xA3:
                and8(getE());
                break;
            case 0xA4:
                and8(getH());
                break;
            case 0xA5:
                and8(getL());
                break;
            case 0xA6:
                and8(ram[getHL()]);
                break;
            case 0xE6:
                and8(n8);
                break;

            /*
             * OR n
             * Logical OR n with register A, put result in A
             */
            case 0xB7:
                or8(getA());
                break;
            case 0xB0:
                or8(getB());
                break;
            case 0xB1:
                or8(getC());
                break;
            case 0xB2:
                or8(getD());
                break;
            case 0xB3:
                or8(getE());
                break;
            case 0xB4:
                or8(getH());
                break;
            case 0xB5:
                or8(getL());
                break;
            case 0xB6:
                or8(ram[getHL()]);
                break;
            case 0xF6:
                or8(n8);
                break;

            /*
             * XOR n
             * Logical OR n with register A, put result in A
             */
            case 0xAF:
                xor8(getA());
                break;
            case 0xA8:
                xor8(getB());
                break;
            case 0xA9:
                xor8(getC());
                break;
            case 0xAA:
                xor8(getD());
                break;
            case 0xAB:
                xor8(getE());
                break;
            case 0xAC:
                xor8(getH());
                break;
            case 0xAD:
                xor8(getL());
                break;
            case 0xAE:
                xor8(ram[getHL()]);
                break;
            case 0xEE:
                xor8(n8);
                break;

            /*
             * CP n
             * compare A with n
             */
            case 0xBF:
                compare(getA());
                break;
            case 0xB8:
                compare(getB());
                break;
            case 0xB9:
                compare(getC());
                break;
            case 0xBA:
                compare(getD());
                break;
            case 0xBB:
                compare(getE());
                break;
            case 0xBC:
                compare(getH());
                break;
            case 0xBD:
                compare(getL());
                break;
            case 0xBE:
                compare(ram[getHL()]);
                break;
            case 0xFE:
                compare(n8);
                break;

            /*
             * INC n
             * increment register n
             */
            case 0x3C:
                setA(add8(getA(), 1));
                break;
            case 0x04:
                setB(add8(getB(), 1));
                break;
            case 0x0C:
                setC(add8(getC(), 1));
                break;
            case 0x14:
                setD(add8(getD(), 1));
                break;
            case 0x1C:
                setE(add8(getE(), 1));
                break;
            case 0x24:
                setH(add8(getH(), 1));
                break;
            case 0x2C:
                setL(add8(getL(), 1));
                break;
            case 0x34:
                ram[getHL()] = add8(ram[getHL()], 1);
                break;

            /*
             * DEC n
             * decrement register n
             */
            case 0x3D:
                setA(sub8(getA(), 1));
                break;
            case 0x05:
                setB(sub8(getB(), 1));
                break;
            case 0x0D:
                setC(sub8(getC(), 1));
                break;
            case 0x15:
                setD(sub8(getD(), 1));
                break;
            case 0x1D:
                setE(sub8(getE(), 1));
                break;
            case 0x25:
                setH(sub8(getH(), 1));
                break;
            case 0x2D:
                setL(sub8(getL(), 1));
                break;
            case 0x35:
                ram[getHL()] = sub8(ram[getHL()], 1);
                break;

            /* <><><><><><><><><><> 16-Bit ALU <><><><><><><><><><> */

            /*
             * ADD HL,n
             * Add n to HL
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

            /*
             * ADD SP,n
             * Add n to stack pointer
             */
            case 0xE8:
                SP = add16(SP, n8);
                break;

            /*
             * INC nn
             * increment register nn
             */
            case 0x03:
                setBC(add16(getBC(), 1));
                break;
            case 0x13:
                setDE(add16(getDE(), 1));
                break;
            case 0x23:
                setHL(add16(getHL(), 1));
                break;
            case 0x33:
                SP = add16(SP, 1);
                break;

            /*
             * DEC nn
             * decrement register nns
             */
            case 0x0B:
                setBC(sub16(getBC(), 1));
                break;
            case 0x1B:
                setDE(sub16(getDE(), 1));
                break;
            case 0x2B:
                setHL(sub16(getHL(), 1));
                break;
            case 0x3B:
                SP = sub16(SP, 1);
                break;

            /* <><><><><><><><><><> Misc <><><><><><><><><><> */

            /*
             * two-byte opcodes
             */
            case 0xCB:

                switch (n8) {

                    /*
                     * RLC
                     * Rotate register n left, old bit 8 in carry flag
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
                    case 0x06:
                        ram[getHL()] = rotateLeft(ram[getHL()]);
                        break;

                    /*
                     * RL n
                     * Rotate register n left through carry flag
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
                    case 0x16:
                        ram[getHL()] = rotateLeftCarry(ram[getHL()]);
                        break;

                    /*
                     * RRC n
                     * Rotate register n right, old bit 0 to carry flag
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
                    case 0x0E:
                        ram[getHL()] = rotateRight(ram[getHL()]);
                        break;

                    /*
                     * RR n
                     * Rotate register n right through carry flag
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
                    case 0x1E:
                        ram[getHL()] = rotateRightCarry(ram[getHL()]);
                        break;

                    /*
                     * SLA n
                     * Shift n left into Carry, LSB of n set to 0
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
                    case 0x26:
                        ram[getHL()] = shiftLeft(ram[getHL()]);
                        break;

                    /*
                     * SRA n
                     * Shift n right into Carry
                     */
                    case 0x2F:
                        setA(shiftRight(getA()));
                        break;
                    case 0x28:
                        setB(shiftRight(getB()));
                        break;
                    case 0x29:
                        setC(shiftRight(getC()));
                        break;
                    case 0x2A:
                        setD(shiftRight(getD()));
                        break;
                    case 0x2B:
                        setE(shiftRight(getE()));
                        break;
                    case 0x2C:
                        setH(shiftRight(getH()));
                        break;
                    case 0x2D:
                        setL(shiftRight(getL()));
                        break;
                    case 0x2E:
                        ram[getHL()] = shiftRight(ram[getHL()]);
                        break;

                    /*
                     * SRL n
                     * Shift n right into Carry, MSB of n set to 0
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
                    case 0x3E:
                        ram[getHL()] = shiftRightLogical(ram[getHL()]);
                        break;

                    /*
                     * SWAP n
                     * swap upper and lower nibbles of n
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
                    case 0x36:
                        ram[getHL()] = swap(ram[getHL()]);

                    /*
                     * BIT b,r
                     * Test bit b in register r
                     */

                    // bit 0
                    case 0x40:
                        bit(0, getB());
                        break;
                    case 0x41:
                        bit(0, getC());
                        break;
                    case 0x42:
                        bit(0, getD());
                        break;
                    case 0x43:
                        bit(0, getE());
                        break;
                    case 0x44:
                        bit(0, getH());
                        break;
                    case 0x45:
                        bit(0, getL());
                        break;
                    case 0x46:
                        bit(0, ram[getHL()]);
                        break;
                    case 0x47:
                        bit(0, getA());
                        break;

                    // bit 1
                    case 0x48:
                        bit(1, getB());
                        break;
                    case 0x49:
                        bit(1, getC());
                        break;
                    case 0x4A:
                        bit(1, getD());
                        break;
                    case 0x4B:
                        bit(1, getE());
                        break;
                    case 0x4C:
                        bit(1, getH());
                        break;
                    case 0x4D:
                        bit(1, getL());
                        break;
                    case 0x4E:
                        bit(1, ram[getHL()]);
                        break;
                    case 0x4F:
                        bit(1, getA());
                        break;

                    // bit 2
                    case 0x50:
                        bit(2, getB());
                        break;
                    case 0x51:
                        bit(2, getC());
                        break;
                    case 0x52:
                        bit(2, getD());
                        break;
                    case 0x53:
                        bit(2, getE());
                        break;
                    case 0x54:
                        bit(2, getH());
                        break;
                    case 0x55:
                        bit(2, getL());
                        break;
                    case 0x56:
                        bit(2, ram[getHL()]);
                        break;
                    case 0x57:
                        bit(2, getA());
                        break;

                    // bit 3
                    case 0x58:
                        bit(3, getB());
                        break;
                    case 0x59:
                        bit(3, getC());
                        break;
                    case 0x5A:
                        bit(3, getD());
                        break;
                    case 0x5B:
                        bit(3, getE());
                        break;
                    case 0x5C:
                        bit(3, getH());
                        break;
                    case 0x5D:
                        bit(3, getL());
                        break;
                    case 0x5E:
                        bit(3, ram[getHL()]);
                        break;
                    case 0x5F:
                        bit(3, getA());
                        break;

                    // bit 4
                    case 0x60:
                        bit(4, getB());
                        break;
                    case 0x61:
                        bit(4, getC());
                        break;
                    case 0x62:
                        bit(4, getD());
                        break;
                    case 0x63:
                        bit(4, getE());
                        break;
                    case 0x64:
                        bit(4, getH());
                        break;
                    case 0x65:
                        bit(4, getL());
                        break;
                    case 0x66:
                        bit(4, ram[getHL()]);
                        break;
                    case 0x67:
                        bit(4, getA());
                        break;

                    // bit 5
                    case 0x68:
                        bit(5, getB());
                        break;
                    case 0x69:
                        bit(5, getC());
                        break;
                    case 0x6A:
                        bit(5, getD());
                        break;
                    case 0x6B:
                        bit(5, getE());
                        break;
                    case 0x6C:
                        bit(5, getH());
                        break;
                    case 0x6D:
                        bit(5, getL());
                        break;
                    case 0x6E:
                        bit(5, ram[getHL()]);
                        break;
                    case 0x6F:
                        bit(5, getA());
                        break;

                    // bit 6
                    case 0x70:
                        bit(6, getB());
                        break;
                    case 0x71:
                        bit(6, getC());
                        break;
                    case 0x72:
                        bit(6, getD());
                        break;
                    case 0x73:
                        bit(6, getE());
                        break;
                    case 0x74:
                        bit(6, getH());
                        break;
                    case 0x75:
                        bit(6, getL());
                        break;
                    case 0x76:
                        bit(6, ram[getHL()]);
                        break;
                    case 0x77:
                        bit(6, getA());
                        break;

                    // bit 7
                    case 0x78:
                        bit(7, getB());
                        break;
                    case 0x79:
                        bit(7, getC());
                        break;
                    case 0x7A:
                        bit(7, getD());
                        break;
                    case 0x7B:
                        bit(7, getE());
                        break;
                    case 0x7C:
                        bit(7, getH());
                        break;
                    case 0x7D:
                        bit(7, getL());
                        break;
                    case 0x7E:
                        bit(7, ram[getHL()]);
                        break;
                    case 0x7F:
                        bit(7, getA());
                        break;

                    /*
                     * SET b,r
                     * Set bit b in register r
                     */

                    // bit 0
                    case 0xC0:
                        setB(set(0, getB()));
                        break;
                    case 0xC1:
                        setC(set(0, getC()));
                        break;
                    case 0xC2:
                        setD(set(0, getD()));
                        break;
                    case 0xC3:
                        setE(set(0, getE()));
                        break;
                    case 0xC4:
                        setH(set(0, getH()));
                        break;
                    case 0xC5:
                        setL(set(0, getL()));
                        break;
                    case 0xC6:
                        ram[getHL()] = set(0, ram[getHL()]);
                        break;
                    case 0xC7:
                        setA(set(0, getA()));
                        break;

                    // bit 1
                    case 0xC8:
                        setB(set(1, getB()));
                        break;
                    case 0xC9:
                        setC(set(1, getC()));
                        break;
                    case 0xCA:
                        setD(set(1, getD()));
                        break;
                    case 0xCB:
                        setE(set(1, getE()));
                        break;
                    case 0xCC:
                        setH(set(1, getH()));
                        break;
                    case 0xCD:
                        setL(set(1, getL()));
                        break;
                    case 0xCE:
                        ram[getHL()] = set(1, ram[getHL()]);
                        break;
                    case 0xCF:
                        setA(set(1, getA()));
                        break;

                    // bit 2
                    case 0xD0:
                        setB(set(2, getB()));
                        break;
                    case 0xD1:
                        setC(set(2, getC()));
                        break;
                    case 0xD2:
                        setD(set(2, getD()));
                        break;
                    case 0xD3:
                        setE(set(2, getE()));
                        break;
                    case 0xD4:
                        setH(set(2, getH()));
                        break;
                    case 0xD5:
                        setL(set(2, getL()));
                        break;
                    case 0xD6:
                        ram[getHL()] = set(2, ram[getHL()]);
                        break;
                    case 0xD7:
                        setA(set(2, getA()));
                        break;

                    // bit 3
                    case 0xD8:
                        setB(set(3, getB()));
                        break;
                    case 0xD9:
                        setC(set(3, getC()));
                        break;
                    case 0xDA:
                        setD(set(3, getD()));
                        break;
                    case 0xDB:
                        setE(set(3, getE()));
                        break;
                    case 0xDC:
                        setH(set(3, getH()));
                        break;
                    case 0xDD:
                        setL(set(3, getL()));
                        break;
                    case 0xDE:
                        ram[getHL()] = set(3, ram[getHL()]);
                        break;
                    case 0xDF:
                        setA(set(3, getA()));
                        break;

                    // bit 4
                    case 0xE0:
                        setB(set(4, getB()));
                        break;
                    case 0xE1:
                        setC(set(4, getC()));
                        break;
                    case 0xE2:
                        setD(set(4, getD()));
                        break;
                    case 0xE3:
                        setE(set(4, getE()));
                        break;
                    case 0xE4:
                        setH(set(4, getH()));
                        break;
                    case 0xE5:
                        setL(set(4, getL()));
                        break;
                    case 0xE6:
                        ram[getHL()] = set(4, ram[getHL()]);
                        break;
                    case 0xE7:
                        setA(set(4, getA()));
                        break;

                    // bit 5
                    case 0xE8:
                        setB(set(5, getB()));
                        break;
                    case 0xE9:
                        setC(set(5, getC()));
                        break;
                    case 0xEA:
                        setD(set(5, getD()));
                        break;
                    case 0xEB:
                        setE(set(5, getE()));
                        break;
                    case 0xEC:
                        setH(set(5, getH()));
                        break;
                    case 0xED:
                        setL(set(5, getL()));
                        break;
                    case 0xEE:
                        ram[getHL()] = set(5, ram[getHL()]);
                        break;
                    case 0xEF:
                        setA(set(5, getA()));
                        break;

                    // bit 6
                    case 0xF0:
                        setB(set(6, getB()));
                        break;
                    case 0xF1:
                        setC(set(6, getC()));
                        break;
                    case 0xF2:
                        setD(set(6, getD()));
                        break;
                    case 0xF3:
                        setE(set(6, getE()));
                        break;
                    case 0xF4:
                        setH(set(6, getH()));
                        break;
                    case 0xF5:
                        setL(set(6, getL()));
                        break;
                    case 0xF6:
                        ram[getHL()] = set(6, ram[getHL()]);
                        break;
                    case 0xF7:
                        setA(set(6, getA()));
                        break;

                    // bit 7
                    case 0xF8:
                        setB(set(7, getB()));
                        break;
                    case 0xF9:
                        setC(set(7, getC()));
                        break;
                    case 0xFA:
                        setD(set(7, getD()));
                        break;
                    case 0xFB:
                        setE(set(7, getE()));
                        break;
                    case 0xFC:
                        setH(set(7, getH()));
                        break;
                    case 0xFD:
                        setL(set(7, getL()));
                        break;
                    case 0xFE:
                        ram[getHL()] = set(7, ram[getHL()]);
                        break;
                    case 0xFF:
                        setA(set(7, getA()));
                        break;

                    /*
                     * RES b,r
                     * Reset bit b in register r
                     */

                    // bit 0
                    case 0x80:
                        setB(reset(0, getB()));
                        break;
                    case 0x81:
                        setC(reset(0, getC()));
                        break;
                    case 0x82:
                        setD(reset(0, getD()));
                        break;
                    case 0x83:
                        setE(reset(0, getE()));
                        break;
                    case 0x84:
                        setH(reset(0, getH()));
                        break;
                    case 0x85:
                        setL(reset(0, getL()));
                        break;
                    case 0x86:
                        ram[getHL()] = reset(0, ram[getHL()]);
                        break;
                    case 0x87:
                        setA(reset(0, getA()));
                        break;

                    // bit 1
                    case 0x88:
                        setB(reset(1, getB()));
                        break;
                    case 0x89:
                        setC(reset(1, getC()));
                        break;
                    case 0x8A:
                        setD(reset(1, getD()));
                        break;
                    case 0x8B:
                        setE(reset(1, getE()));
                        break;
                    case 0x8C:
                        setH(reset(1, getH()));
                        break;
                    case 0x8D:
                        setL(reset(1, getL()));
                        break;
                    case 0x8E:
                        ram[getHL()] = reset(1, ram[getHL()]);
                        break;
                    case 0x8F:
                        setA(reset(1, getA()));
                        break;

                    // bit 2
                    case 0x90:
                        setB(reset(2, getB()));
                        break;
                    case 0x91:
                        setC(reset(2, getC()));
                        break;
                    case 0x92:
                        setD(reset(2, getD()));
                        break;
                    case 0x93:
                        setE(reset(2, getE()));
                        break;
                    case 0x94:
                        setH(reset(2, getH()));
                        break;
                    case 0x95:
                        setL(reset(2, getL()));
                        break;
                    case 0x96:
                        ram[getHL()] = reset(2, ram[getHL()]);
                        break;
                    case 0x97:
                        setA(reset(2, getA()));
                        break;

                    // bit 3
                    case 0x98:
                        setB(reset(3, getB()));
                        break;
                    case 0x99:
                        setC(reset(3, getC()));
                        break;
                    case 0x9A:
                        setD(reset(3, getD()));
                        break;
                    case 0x9B:
                        setE(reset(3, getE()));
                        break;
                    case 0x9C:
                        setH(reset(3, getH()));
                        break;
                    case 0x9D:
                        setL(reset(3, getL()));
                        break;
                    case 0x9E:
                        ram[getHL()] = reset(3, ram[getHL()]);
                        break;
                    case 0x9F:
                        setA(reset(3, getA()));
                        break;

                    // bit 4
                    case 0xA0:
                        setB(reset(4, getB()));
                        break;
                    case 0xA1:
                        setC(reset(4, getC()));
                        break;
                    case 0xA2:
                        setD(reset(4, getD()));
                        break;
                    case 0xA3:
                        setE(reset(4, getE()));
                        break;
                    case 0xA4:
                        setH(reset(4, getH()));
                        break;
                    case 0xA5:
                        setL(reset(4, getL()));
                        break;
                    case 0xA6:
                        ram[getHL()] = reset(4, ram[getHL()]);
                        break;
                    case 0xA7:
                        setA(reset(4, getA()));
                        break;

                    // bit 5
                    case 0xA8:
                        setB(reset(5, getB()));
                        break;
                    case 0xA9:
                        setC(reset(5, getC()));
                        break;
                    case 0xAA:
                        setD(reset(5, getD()));
                        break;
                    case 0xAB:
                        setE(reset(5, getE()));
                        break;
                    case 0xAC:
                        setH(reset(5, getH()));
                        break;
                    case 0xAD:
                        setL(reset(5, getL()));
                        break;
                    case 0xAE:
                        ram[getHL()] = reset(5, ram[getHL()]);
                        break;
                    case 0xAF:
                        setA(reset(5, getA()));
                        break;

                    // bit 6
                    case 0xB0:
                        setB(reset(6, getB()));
                        break;
                    case 0xB1:
                        setC(reset(6, getC()));
                        break;
                    case 0xB2:
                        setD(reset(6, getD()));
                        break;
                    case 0xB3:
                        setE(reset(6, getE()));
                        break;
                    case 0xB4:
                        setH(reset(6, getH()));
                        break;
                    case 0xB5:
                        setL(reset(6, getL()));
                        break;
                    case 0xB6:
                        ram[getHL()] = reset(6, ram[getHL()]);
                        break;
                    case 0xB7:
                        setA(reset(6, getA()));
                        break;

                    // bit 7
                    case 0xB8:
                        setB(reset(7, getB()));
                        break;
                    case 0xB9:
                        setC(reset(7, getC()));
                        break;
                    case 0xBA:
                        setD(reset(7, getD()));
                        break;
                    case 0xBB:
                        setE(reset(7, getE()));
                        break;
                    case 0xBC:
                        setH(reset(7, getH()));
                        break;
                    case 0xBD:
                        setL(reset(7, getL()));
                        break;
                    case 0xBE:
                        ram[getHL()] = reset(7, ram[getHL()]);
                        break;
                    case 0xBF:
                        setA(reset(7, getA()));
                        break;

                    default:
                        break;
                }
                PC += 1;
                break;

            /*
             * DAA
             * decimal adjust register A
             * adjust so that correct BCD can be obtained
             */
            case 0x27: // TBI
                break;

            /*
             * CPL
             * complement register A
             */
            case 0x2F:
                setSub(true);
                setHalf(true);
                setA(~getA());
                break;

            /*
             * CCF
             * complement carry flag
             */
            case 0x3F:
                setCarry(~getCarry());
                setSub(false);
                setHalf(false);
                break;

            /*
             * SCF
             * set carry flag
             */
            case 0x37:
                setCarry(true);
                setSub(false);
                setHalf(false);
                break;

            /*
             * NOP
             * no operation
             */
            case 0x00:
                break;

            /*
             * HALT
             * power down CPU until interrupt
             */
            case 0x76: // TBI
                break;

            /*
             * STOP
             * halt CPU and LCD display until button pressed
             */
            case 0x10: // TBI
                break;

            /*
             * DI
             * disable interrupts
             */
            case 0xF3: // TBI
                break;

            /*
             * EI
             * enable interrupts
             */
            case 0xFB: // TBI
                break;

            /* <><><><><><><><><><> Rotates & Shifts <><><><><><><><><><> */

            /*
             * RLCA
             * Rotate A left, old bit 7 to carry flag
             */
            case 0x07:
                setA(rotateLeft(getA()));
                break;

            /*
             * RLA
             * Rotate A left through carry flag
             */
            case 0x17:
                setA(rotateLeftCarry(getA()));
                break;

            /*
             * RRCA
             * Rotate A right, old bit 0 to carry flag
             */
            case 0x0F:
                setA(rotateRight(getA()));
                break;

            /*
             * RRA
             * Rotate A right through carry flag
             */
            case 0x1F:
                setA(rotateRightCarry(getA()));
                break;

            /* <><><><><><><><><><> JUMPS <><><><><><><><><><> */

            /*
             * JP nn
             * Jump to address nn
             */
            case 0xC3:
                PC = n16;
                break;

            /*
             * JP cc,nn
             * Jump to address nn if following condition is true:
             * cc = NZ, Jump if Z flag is reset
             * cc = Z, Jump if Z flag is set
             * cc = NC, Jump if C flag is reset
             * cc = C, Jump if C flag is set
             */
            case 0xC2:
                if (!getZero()) PC = n16;
                break;
            case 0xCA:
                if (getZero()) PC = n16;
                break;
            case 0xD2:
                if (!getCarry()) PC = n16;
                break;
            case 0xDA:
                if (getCarry()) PC = n16;
                break;

            /*
             * JP (HL)
             * Jump to address contained in HL
             */
            case 0xE9:
                PC = ram[getHL()];
                break;

            /*
             * JR n
             * Add n to current address and jump to it
             */
            case 0x18:
                PC += n8;
                break;

            /*
             * JR cc,n
             * Add n to current address and jump to it if:
             * cc = NZ, jump if Z flag is reset
             * cc = Z, jump if Z flag is set
             * cc = NC, jump if C flag is reset
             * cc = C, jump if C flag is set
             */
            case 0x20:
                if (!getZero()) PC += n8;
                break;
            case 0x28:
                if (getZero()) PC += n8;
                break;
            case 0x30:
                if (!getCarry()) PC += n8;
                break;
            case 0x38:
                if (getCarry()) PC += n8;
                break;

            /* <><><><><><><><><><> CALLS <><><><><><><><><><> */

            /*
             * CALL nn
             * Push address of next instruction onto stack and then
             * jump to address nn
             */
            case 0xCD:
                push(static_cast<unsigned short>(PC + 1));
                PC = n16;
                break;

            /*
             * CALL cc,nn
             * Call address nn if the following condition is true:
             * cc = NZ, call if Z flag is reset
             * cc = Z, call if Z flag is set
             * cc = NZ, call if C flag is reset
             * cc = C, call if C flag is set
             */
            case 0xC4:
                if (!getZero()) {
                    push(static_cast<unsigned short>(PC + 1));
                    PC = n16;
                }
                break;
            case 0xCC:
                if (getZero()) {
                    push(static_cast<unsigned short>(PC + 1));
                    PC = n16;
                }
                break;
            case 0xD4:
                if (!getCarry()) {
                    push(static_cast<unsigned short>(PC + 1));
                    PC = n16;
                }
                break;
            case 0xDC:
                if (getCarry()) {
                    push(static_cast<unsigned short>(PC + 1));
                    PC = n16;
                }
                break;

            /* <><><><><><><><><><> RESTARTS <><><><><><><><><><> */

            /*
             * RST n
             * Push present address onto stack
             * Jump to address 0x0000 + n
             */
            case 0xC7:
                reset(0x00);
                break;
            case 0xCF:
                reset(0x08);
                break;
            case 0xD7:
                reset(0x10);
                break;
            case 0xDF:
                reset(0x18);
                break;
            case 0xE7:
                reset(0x20);
                break;
            case 0xEF:
                reset(0x28);
                break;
            case 0xF7:
                reset(0x30);
                break;
            case 0xFF:
                reset(0x38);
                break;

            /* <><><><><><><><><><> RETURNS <><><><><><><><><><> */

            /*
             * RET
             * Pop two bytes from stack & jump to that address
             */
            case 0xC9:
                PC = pop();
                break;

            /*
             * RET cc
             * Return if following condition is true:
             * cc = NZ, return if Z flag is reset
             * cc = Z, return if Z flag is set
             * cc = NC, return if C flag is reset
             * cc = C, return if C flag is set
             */
            case 0xC0:
                if (!getZero()) PC = pop();
                break;
            case 0xC8:
                if (getZero()) PC = pop();
                break;
            case 0xD0:
                if (!getCarry()) PC = pop();
                break;
            case 0xD8:
                if (getCarry()) PC = pop();
                break;

            /*
             * RETI
             * Pop two bytes from stack & jump to that address then
             * enable interrupts
             */
            case 0xD9:
                PC = pop();
                interrupts = true;
                break;

            default:
                break;
        }
    }

    unsigned char add8(unsigned char a, unsigned char b) {
        short sum = a + b;
        auto a_nib = a & 0xF; // lower nibble of a
        auto b_nib = b & 0xF; // lower nibble of b
        auto sum_nib = a_nib + b_nib;
        setZero(sum == 0);
        setSub(false);
        setHalf(((sum_nib >> 4) & 0x1) == 1);
        setCarry((sum >> 8 & 0x1) == 1);
        return static_cast<unsigned char>(sum);
    }

    unsigned char sub8(unsigned char a, unsigned char b) {
        return add8(a, static_cast<unsigned char>(~b + 1));
    }

    unsigned short add16(unsigned short a, unsigned short b) {
        unsigned int sum = a + b;
        auto a_nib3 = a & 0xFFF; // lower 3 nibbles of a
        auto b_nib3 = b & 0xFFF; // lower 3 nibbles of b
        auto sum_nib3 = a_nib3 + b_nib3;
        setSub(false);
        setHalf(((sum_nib3 >> 12) & 0x1) == 1);
        setCarry(((sum >> 16) & 0x1) == 1);
        return static_cast<unsigned short>(sum);
    }

    unsigned short sub16(unsigned short a, unsigned short b) {
        return add16(a, static_cast<unsigned short>(~b + 1));
    }

    unsigned char swap(unsigned char val) {
        return static_cast<unsigned char>(((val << 4) & 0xF0) | ((val >> 4) & 0x0F));
    }

    void compare(unsigned char value) {
        auto value_nibble = value & 0xF;
        auto A_nibble = getA() & 0xF;
        setZero(getA() - value == 0);
        setSub(true);
        setHalf(A_nibble - value < 0);
        setCarry(getA() - value < 0);
    }

    unsigned char rotateLeft(unsigned char reg) {
        auto bit7 = static_cast<unsigned char>(reg >> 7 & 0x1);
        unsigned char rotate = reg << 1 | bit7;
        setZero(~rotate);
        setSub(false);
        setHalf(false);
        setCarry(bit7);
        return rotate;
    }

    unsigned char rotateLeftCarry(unsigned char reg) {
        auto bit7 = static_cast<unsigned char>(reg >> 7 & 0x1);
        unsigned char rotate = reg << 1 | getCarry();
        setZero(~rotate);
        setSub(false);
        setHalf(false);
        setCarry(bit7);
        return rotate;
    }

   unsigned char rotateRight(unsigned char reg) {
        auto bit0 = static_cast<unsigned char>(reg & 0x1);
        auto rotate = static_cast<unsigned char>((reg >> 1 & 0x1) | (bit0 << 7));
        setZero(~rotate);
        setSub(false);
        setHalf(false);
        setCarry(bit0);
        return rotate;
    }

    unsigned char rotateRightCarry(unsigned char reg) {
        auto bit0 = static_cast<unsigned char>(reg & 0x1);
        auto rotate = static_cast<unsigned char>((reg >> 1 & 0x1) | (getCarry() << 7));
        setZero(~rotate);
        setSub(false);
        setHalf(false);
        setCarry(bit0);
        return rotate;
    }

    unsigned char shiftLeft(unsigned char reg) {
        auto bit7 = static_cast<unsigned char>((reg >> 7) & 0x1);
        auto shift = static_cast<unsigned char>((reg << 1) & 0xFE);
        setZero(shift == 0);
        setSub(false);
        setHalf(false);
        setCarry(bit7);
        return shift;
    }

    unsigned char shiftRight(unsigned char reg) {
        auto bit0 = static_cast<unsigned char>(reg & 0x1);
        unsigned char shift = reg >> 1;
        setZero(shift == 0);
        setSub(false);
        setHalf(false);
        setCarry(bit0);
        return shift;
    }

    unsigned char shiftRightLogical(unsigned char reg) {
        auto shift = shiftRight(reg);
        return static_cast<unsigned char>(shift & 0x7F);
    }

    void and8(unsigned char value) {
        auto temp = value & getA();
        setZero(temp == 0);
        setSub(false);
        setHalf(true);
        setCarry(false);
        setA(temp);
    }

    void or8(unsigned char value) {
        auto temp = value | getA();
        setZero(temp == 0);
        setSub(false);
        setHalf(false);
        setCarry(false);
        setA(temp);
    }

    void xor8(unsigned char value) {
        auto temp = value ^ getA();
        setZero(temp == 0);
        setSub(false);
        setHalf(false);
        setCarry(false);
        setA(temp);
    }

    /* <><><><><><><><><><> Bit Operations <><><><><><><><><><> */

    void bit(unsigned char bit, unsigned char reg) {
        auto bitX = static_cast<unsigned char>((reg >> bit) & 0x1);
        setZero(bitX == 0);
        setSub(false);
        setHalf(true);
    }

    unsigned char set(unsigned char bit, unsigned char reg) {
        return static_cast<unsigned char>(reg | (1 << bit));
    }

    unsigned char reset(unsigned char bit, unsigned char reg) {
        auto mask = static_cast<unsigned char>(0xFF ^ (1 << bit));
        return reg & mask;
    }

    /* <><><><><><><><><><> Stack Operations <><><><><><><><><><> */
    void push(unsigned short value) {
        auto lo = static_cast<unsigned char>(value & 0xFF);
        auto hi = static_cast<unsigned char>((value >> 8) & 0xFF);
        ram[SP] = lo;
        ram[SP + 1] = hi;
        SP -= 2;
    }

    void reset(unsigned char value) {
        push(value);
        PC = value;
    }

    unsigned short pop() {
        SP += 2;
        unsigned char lo = ram[SP];
        unsigned char hi = ram[SP + 1];
        return static_cast<unsigned short>((hi << 8) | lo);
    }

    // register write functions
    void setA(unsigned char value) { A = value; }
    void setB(unsigned char value) { BC = value << 8; }
    void setC(unsigned char value) { BC = value; }
    void setD(unsigned char value) { DE = value << 8; }
    void setE(unsigned char value) { DE = value; }
    void setH(unsigned char value) { HL = value << 8; }
    void setL(unsigned char value) { HL = value; }
    void setBC(unsigned short value) { BC = value; }
    void setDE(unsigned short value) { DE = value; }
    void setHL(unsigned short value) { HL = value; }
    void decHL() { HL--; }
    void incHL() { HL++; }

    // register read functions
    unsigned char getA() { return A; }
    unsigned char getB() { return static_cast<unsigned char>((BC >> 8) & 0xFF); }
    unsigned char getC() { return static_cast<unsigned char>(BC & 0xFF); }
    unsigned short getBC() { return BC; }
    unsigned char getD() { return static_cast<unsigned char>((DE >> 8) & 0xFF); }
    unsigned char getE() { return static_cast<unsigned char>(DE & 0xFF); }
    unsigned short getDE() { return DE; }
    unsigned char getF() { return F; }
    unsigned char getH() { return static_cast<unsigned char>((HL >> 8) & 0xFF); }
    unsigned char getL() { return static_cast<unsigned char>(HL & 0xFF); }
    unsigned short getHL() { return HL; }

    // flag write functions
    void setZero(bool value) { F = static_cast<unsigned char>((value) ? F | 0x80 : F & 0x7F); }
    void setSub(bool value) { F = static_cast<unsigned char>((value) ? F | 0x40 : F & 0xBF); }
    void setHalf(bool value) { F = static_cast<unsigned char>((value) ? F | 0x20 : F & 0xDF); }
    void setCarry(bool value) { F = static_cast<unsigned char>((value) ? F | 0x10 : F & 0xEF); }

    // flag read functions
    bool getZero() { return static_cast<bool>((F & 0x80) >> 7); }
    bool getSub() { return static_cast<bool>((F & 0x40) >> 6); }
    bool getHalf() { return static_cast<bool>((F & 0x20) >> 5); }
    bool getCarry() { return static_cast<bool>((F & 0x10) >> 4); }
};