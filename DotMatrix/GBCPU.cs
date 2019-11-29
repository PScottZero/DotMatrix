using System;
using System.Collections.Generic;
using System.Text;

namespace DotMatrix
{
    class GBCPU
    {
        ushort AF, BC, DE, HL, PC, SP;
        byte[] mem = new byte[0xFFFF];
        byte[] cart = new byte[0x200000];
        bool IME;

        // Initialize registers
        GBCPU()
        {
            AF = BC = DE = HL = PC = 0;
            SP = 0xFFFE;
            IME = true;
        }

        // Loads cartidge data into memory
        public void LoadCartridge()
        {
            // TODO: Implement load cartridge
        }

        // Loads gaemboy bootstrap
        public void LoadBootstrap()
        {
            // TODO: Implement load bootstrap
        }

        // run emulator
        public void Run()
        {
            // TODO: Implement run method
        }

        // decode instruction
        void Decode()
        {
            switch (mem[PC])
            {
                // -------------------- 8-BIT TRANSFER AND INPUT/OUTPUT INSTRUCTIONS --------------------

                // LD r, r'
                // Load contents of register r' into register r

                // destination A
                case 0x7F:
                    SetA(GetA());
                    break;
                case 0x78:
                    SetA(GetB());
                    break;
                case 0x79:
                    SetA(GetC());
                    break;
                case 0x7A:
                    SetA(GetD());
                    break;
                case 0x7B:
                    SetA(GetE());
                    break;
                case 0x7C:
                    SetA(GetH());
                    break;
                case 0x7D:
                    SetA(GetL());
                    break;
                case 0x47:
                    SetB(GetA());
                    break;

                // destination B
                case 0x40:
                    SetB(GetB());
                    break;
                case 0x41:
                    SetB(GetC());
                    break;
                case 0x42:
                    SetB(GetD());
                    break;
                case 0x43:
                    SetB(GetE());
                    break;
                case 0x44:
                    SetB(GetH());
                    break;
                case 0x45:
                    SetB(GetL());
                    break;

                // destination C
                case 0x4F:
                    SetC(GetA());
                    break;
                case 0x48:
                    SetC(GetB());
                    break;
                case 0x49:
                    SetC(GetC());
                    break;
                case 0x4A:
                    SetC(GetD());
                    break;
                case 0x4B:
                    SetC(GetE());
                    break;
                case 0x4C:
                    SetC(GetH());
                    break;
                case 0x4D:
                    SetC(GetL());
                    break;

                // destination D
                case 0x57:
                    SetD(GetA());
                    break;
                case 0x50:
                    SetD(GetB());
                    break;
                case 0x51:
                    SetD(GetC());
                    break;
                case 0x52:
                    SetD(GetD());
                    break;
                case 0x53:
                    SetD(GetE());
                    break;
                case 0x54:
                    SetD(GetH());
                    break;
                case 0x55:
                    SetD(GetL());
                    break;

                // destination E
                case 0x5F:
                    SetE(GetA());
                    break;
                case 0x58:
                    SetE(GetB());
                    break;
                case 0x59:
                    SetE(GetC());
                    break;
                case 0x5A:
                    SetE(GetD());
                    break;
                case 0x5B:
                    SetE(GetE());
                    break;
                case 0x5C:
                    SetE(GetH());
                    break;
                case 0x5D:
                    SetE(GetL());
                    break;

                // destination H
                case 0x67:
                    SetH(GetA());
                    break;
                case 0x60:
                    SetH(GetB());
                    break;
                case 0x61:
                    SetH(GetC());
                    break;
                case 0x62:
                    SetH(GetD());
                    break;
                case 0x63:
                    SetH(GetE());
                    break;
                case 0x64:
                    SetH(GetH());
                    break;
                case 0x65:
                    SetH(GetL());
                    break;

                // destination L
                case 0x6F:
                    SetL(GetA());
                    break;
                case 0x68:
                    SetL(GetB());
                    break;
                case 0x69:
                    SetL(GetC());
                    break;
                case 0x6A:
                    SetL(GetD());
                    break;
                case 0x6B:
                    SetL(GetE());
                    break;
                case 0x6C:
                    SetL(GetH());
                    break;
                case 0x6D:
                    SetL(GetL());
                    break;

                // LD r, n
                // Load 8-bit immediate n into register r
                case 0x3E:
                    SetA(mem[PC + 1]); PC++;
                    break;
                case 0x06:
                    SetB(mem[PC + 1]); PC++;
                    break;
                case 0x0E:
                    SetC(mem[PC + 1]); PC++;
                    break;
                case 0x16:
                    SetD(mem[PC + 1]); PC++;
                    break;
                case 0x1E:
                    SetE(mem[PC + 1]); PC++;
                    break;
                case 0x26:
                    SetH(mem[PC + 1]); PC++;
                    break;
                case 0x2E:
                    SetL(mem[PC + 1]); PC++;
                    break;

                // LD r, (HL)
                // Load memory at address HL into register r
                case 0x7E:
                    SetA(mem[GetHL()]);
                    break;
                case 0x46:
                    SetB(mem[GetHL()]);
                    break;
                case 0x4E:
                    SetC(mem[GetHL()]);
                    break;
                case 0x56:
                    SetD(mem[GetHL()]);
                    break;
                case 0x5E:
                    SetE(mem[GetHL()]);
                    break;
                case 0x66:
                    SetH(mem[GetHL()]);
                    break;
                case 0x6E:
                    SetL(mem[GetHL()]);
                    break;

                // LD (HL), r
                // Load register r into memory at address HL
                case 0x77:
                    mem[GetHL()] = GetA();
                    break;
                case 0x70:
                    mem[GetHL()] = GetB();
                    break;
                case 0x71:
                    mem[GetHL()] = GetC();
                    break;
                case 0x72:
                    mem[GetHL()] = GetD();
                    break;
                case 0x73:
                    mem[GetHL()] = GetE();
                    break;
                case 0x74:
                    mem[GetHL()] = GetH();
                    break;
                case 0x75:
                    mem[GetHL()] = GetL();
                    break;

                // LD (HL), n
                // Load 8-bit immediate n into memory at address HL
                case 0x36:
                    mem[GetHL()] = mem[PC + 1];
                    PC++;
                    break;

                // LD A, (BC)
                // Load memory at address BC into register A
                case 0x0A:
                    SetA(mem[GetBC()]);
                    break;

                // LD A, (DE)
                // Load memory at address DE into register A
                case 0x1A:
                    SetA(mem[GetDE()]);
                    break;

                // LD a, (C)
                // Load memory at address 0xFF00 + [register C] into register A
                case 0xF2:
                    SetA(mem[0xFF00 + GetC()]);
                    break;

                // LD (C), A
                // Load register A into memory at address 0xFF00 + [register C]
                case 0xE2:
                    mem[0xFF00 + GetC()] = GetA();
                    break;

                // LD A, (n)
                // Load memory at address 0xFF00 + [8-bit immediate n] into register A
                case 0xF0:
                    SetA(mem[0xFF00 + mem[PC + 1]]); PC++;
                    break;

                // LD (n), A
                // Load register A into memory at address 0xFF00 + [8-bit immediate n]
                case 0xE0:
                    mem[0xFF00 + mem[PC + 1]] = GetA(); PC++;
                    break;

                // LD A, (nn)
                // Load memory at address specified by 16-bit immediate nn into register A
                case 0xFA:
                    SetA(mem[(mem[PC + 2] << 8) | mem[PC + 1]]); PC += 2;
                    break;

                // LD (nn), A
                // Load register A into memory at address specified by 16-bit immediate nn
                case 0xEA:
                    mem[(mem[PC + 2] << 8) | mem[PC + 1]] = GetA(); PC += 2;
                    break;

                // LD A, (HLI)
                // Load memory at address HL into register A, then increment HL
                case 0x2A:
                    SetA(mem[GetHL()]);
                    SetHL((ushort)(GetHL() + 1));
                    break;

                // LD A, (HLD)
                // Load memory at address HL into register A, then decrement HL
                case 0x3A:
                    SetA(mem[GetHL()]);
                    SetHL((ushort)(GetHL() - 1));
                    break;

                // LD (BC), A
                // Load register A into memory at address BC
                case 0x02:
                    mem[GetBC()] = GetA();
                    break;

                // LD (DE), A
                // Load register A into memory at address DE
                case 0x12:
                    mem[GetDE()] = GetA();
                    break;

                // LD (HLI), A
                // Load register A into memory at address HL, then increment HL
                case 0x22:
                    mem[GetHL()] = GetA();
                    SetHL((ushort)(GetHL() + 1));
                    break;

                // LD (HLD), A
                // Load register A into memory at address HL, then decrement HL
                case 0x32:
                    mem[GetHL()] = GetA();
                    SetHL((ushort)(GetHL() - 1));
                    break;



                // -------------------- 16-BIT TRANSFER INSTRUCTIONS --------------------

                // LD dd, nn
                // Load 2 byte immediate nn into register pair dd
                case 0x01:
                    SetBC((ushort)((mem[PC + 2] << 8) | mem[PC + 1]));
                    PC += 2;
                    break;
                case 0x11:
                    SetDE((ushort)((mem[PC + 2] << 8) | mem[PC + 1]));
                    PC += 2;
                    break;
                case 0x21:
                    SetHL((ushort)((mem[PC + 2] << 8) | mem[PC + 1]));
                    PC += 2;
                    break;
                case 0x31:
                    SP = ((ushort)((mem[PC + 2] << 8) | mem[PC + 1]));
                    PC += 2;
                    break;

                // LD SP, HL
                // Load register HL into SP
                case 0xF9:
                    SP = GetHL();
                    break;

                // PUSH qq
                // Push register qq onto stack
                case 0xC5:
                    mem[SP - 1] = GetB();
                    mem[SP - 2] = GetC();
                    SP -= 2;
                    break;
                case 0xD5:
                    mem[SP - 1] = GetD();
                    mem[SP - 2] = GetE();
                    SP -= 2;
                    break;
                case 0xE5:
                    mem[SP - 1] = GetH();
                    mem[SP - 2] = GetL();
                    SP -= 2;
                    break;
                case 0xF5:
                    mem[SP - 1] = GetA();
                    mem[SP - 2] = GetF();
                    SP -= 2;
                    break;

                // POP qq
                // Pop contents of stack into register pair qq
                case 0xC1:
                    SetC(mem[SP]);
                    SetB(mem[SP + 1]);
                    SP += 2;
                    break;
                case 0xD1:
                    SetE(mem[SP]);
                    SetD(mem[SP + 1]);
                    SP += 2;
                    break;
                case 0xE1:
                    SetL(mem[SP]);
                    SetH(mem[SP + 1]);
                    SP += 2;
                    break;
                case 0xF1:
                    SetF(mem[SP]);
                    SetA(mem[SP + 1]);
                    SP += 2;
                    break;

                // LDHL SP, e
                // Add 8-bit immediate e to SP and store result in HL
                case 0xF8:
                    SetHL(Add16(GetHL(), mem[PC + 1]));
                    PC++;
                    break;

                // LD (nn), SP
                // Load stack pointer into memory at address specified by 16-bit immediate nn
                case 0x08:
                    mem[(mem[PC + 2] << 8) | mem[PC + 1]] = SP;
                    PC += 2;
                    break;



                // -------------------- 8-BIT ARITHMETIC AND LOGICAL OPERATION INSTRUCTIONS --------------------

                // ADD A, r
                // Add register r to register A and store result in A
                case 0x87:
                    SetA(Add8(GetA(), GetA()));
                    break;
                case 0x80:
                    SetA(Add8(GetA(), GetB()));
                    break;
                case 0x81:
                    SetA(Add8(GetA(), GetC()));
                    break;
                case 0x82:
                    SetA(Add8(GetA(), GetD()));
                    break;
                case 0x83:
                    SetA(Add8(GetA(), GetE()));
                    break;
                case 0x84:
                    SetA(Add8(GetA(), GetH()));
                    break;
                case 0x85:
                    SetA(Add8(GetA(), GetL()));
                    break;

                // ADD A, n
                // Add 8-bit immediate n with register A and store result in A
                case 0xC6:
                    SetA(Add8(GetA(), mem[PC + 1]));
                    PC++;
                    break;

                // ADD A, (HL)
                // Add memory at address HL to register A and store result in A
                case 0x86:
                    SetA(Add8(GetA(), mem[GetHL()]));
                    break;

                // ADC A, r
                // Add A, register r, and carry flag together and store result in A
                case 0x8F:
                    SetA(Add8(GetA(), GetA() + GetCarry()));
                    break;
                case 0x88:
                    SetA(Add8(GetA(), GetB() + GetCarry()));
                    break;
                case 0x89:
                    SetA(Add8(GetA(), GetC() + GetCarry()));
                    break;
                case 0x8A:
                    SetA(Add8(GetA(), GetD() + GetCarry()));
                    break;
                case 0x8B:
                    SetA(Add8(GetA(), GetE() + GetCarry()));
                    break;
                case 0x8C:
                    SetA(Add8(GetA(), GetH() + GetCarry()));
                    break;
                case 0x8D:
                    SetA(Add8(GetA(), GetL() + GetCarry()));
                    break;

                // ADC A, n
                // Add A, 8-bit immediate n, and carry flag together and store result in A
                case 0xCE:
                    SetA(Add8(GetA(), mem[PC + 1] + GetCarry()));
                    PC++;
                    break;

                // ADC A, (HL)
                // Add A, memory at address HL, and carry flag together and store result in A
                case 0x8E:
                    SetA(Add8(GetA(), mem[GetHL()] + GetCarry()));
                    break;

                // SUB r
                // Subtract register r from register A and store result in A
                case 0x97:
                    SetA(Sub8(GetA(), GetA()));
                    break;
                case 0x90:
                    SetA(Sub8(GetA(), GetB()));
                    break;
                case 0x91:
                    SetA(Sub8(GetA(), GetC()));
                    break;
                case 0x92:
                    SetA(Sub8(GetA(), GetD()));
                    break;
                case 0x93:
                    SetA(Sub8(GetA(), GetE()));
                    break;
                case 0x94:
                    SetA(Sub8(GetA(), GetH()));
                    break;
                case 0x95:
                    SetA(Sub8(GetA(), GetL()));
                    break;

                // SUB n
                // Subtract 8-bit immediate n from register A and store result in A
                case 0xD6:
                    SetA(Sub8(GetA(), mem[PC + 1]));
                    PC++;
                    break;

                // SUB (HL)
                // Subtract memory at address HL from register A and store result in 
                case 0x96:
                    SetA(Sub8(GetA(), mem[GetHL()]));
                    break;

                // SBC A, r
                // Subtract register r and carry flag from register A and store result in A
                case 0x9F:
                    SetA(Sub8(GetA(), GetA() + GetCarry()));
                    break;
                case 0x98:
                    SetA(Sub8(GetA(), GetB() + GetCarry()));
                    break;
                case 0x99:
                    SetA(Sub8(GetA(), GetC() + GetCarry()));
                    break;
                case 0x9A:
                    SetA(Sub8(GetA(), GetD() + GetCarry()));
                    break;
                case 0x9B:
                    SetA(Sub8(GetA(), GetE() + GetCarry()));
                    break;
                case 0x9C:
                    SetA(Sub8(GetA(), GetH() + GetCarry()));
                    break;
                case 0x9D:
                    SetA(Sub8(GetA(), GetL() + GetCarry()));
                    break;

                // SBC A, n
                // Subtract 8-bit immediate n and carry flag from register A and store result in A
                case 0xDE:
                    SetA(Sub8(GetA(), mem[PC + 1] + GetCarry()));
                    PC++;
                    break;

                // SBC A, (HL)
                // Subtract memory at address HL and carry flag from register A and store result in A
                case 0x9E:
                    SetA(Sub8(GetA(), mem[GetHL()] + GetCarry()));
                    break;

                // AND r
                // And register A and register r together and store result in A
                case 0xA7:
                    SetA(And8(GetA(), GetA()));
                    break;
                case 0xA0:
                    SetA(And8(GetA(), GetB()));
                    break;
                case 0xA1:
                    SetA(And8(GetA(), GetC()));
                    break;
                case 0xA2:
                    SetA(And8(GetA(), GetD()));
                    break;
                case 0xA3:
                    SetA(And8(GetA(), GetE()));
                    break;
                case 0xA4:
                    SetA(And8(GetA(), GetH()));
                    break;
                case 0xA5:
                    SetA(And8(GetA(), GetL()));
                    break;

                // AND n
                // And register A and 8-bit immediate n together and store result in A
                case 0xE6:
                    SetA(And8(GetA(), mem[PC + 1]));
                    PC++;
                    break;

                // AND (HL)
                // And register A and memory at address HL together and store result in A
                case 0xA6:
                    SetA(And8(GetA(), mem[GetHL()]));
                    break;

                // OR r
                // Or register A and register r together and store result in A
                case 0xB7:
                    SetA(Or8(GetA(), GetA()));
                    break;
                case 0xB0:
                    SetA(Or8(GetA(), GetB()));
                    break;
                case 0xB1:
                    SetA(Or8(GetA(), GetC()));
                    break;
                case 0xB2:
                    SetA(Or8(GetA(), GetD()));
                    break;
                case 0xB3:
                    SetA(Or8(GetA(), GetE()));
                    break;
                case 0xB4:
                    SetA(Or8(GetA(), GetH()));
                    break;
                case 0xB5:
                    SetA(Or8(GetA(), GetL()));
                    break;

                // OR n
                // Or register A and 8-bit immediate n together and store result in A
                case 0xF6:
                    SetA(Or8(GetA(), mem[PC + 1]));
                    PC++;
                    break;

                // OR (HL)
                // Or register A and memory at address HL together and store result in A
                case 0xB6:
                    SetA(Or8(GetA(), mem[GetHL()]));
                    break;

                // XOR r
                // Xor register A and register r together and store result in A
                case 0xAF:
                    SetA(Xor8(GetA(), GetA()));
                    break;
                case 0xA8:
                    SetA(Xor8(GetA(), GetB()));
                    break;
                case 0xA9:
                    SetA(Xor8(GetA(), GetC()));
                    break;
                case 0xAA:
                    SetA(Xor8(GetA(), GetD()));
                    break;
                case 0xAB:
                    SetA(Xor8(GetA(), GetE()));
                    break;
                case 0xAC:
                    SetA(Xor8(GetA(), GetH()));
                    break;
                case 0xAD:
                    SetA(Xor8(GetA(), GetL()));
                    break;

                // XOR n
                // Xor register A and 8-bit immediate n together and store result in A
                case 0xEE:
                    SetA(Xor8(GetA(), mem[PC + 1]));
                    PC++;
                    break;

                // XOR (HL)
                // Xor register A and memory at address HL together and store result in A
                case 0xAE:
                    SetA(Xor8(GetA(), mem[GetHL()]));
                    break;

                // CP r
                // Compare register A and register R
                case 0xBF:
                    Sub8(GetA(), GetA());
                    break;
                case 0xB8:
                    Sub8(GetA(), GetB());
                    break;
                case 0xB9:
                    Sub8(GetA(), GetC());
                    break;
                case 0xBA:
                    Sub8(GetA(), GetD());
                    break;
                case 0xBB:
                    Sub8(GetA(), GetE());
                    break;
                case 0xBC:
                    Sub8(GetA(), GetH());
                    break;
                case 0xBD:
                    Sub8(GetA(), GetL());
                    break;

                // CP n
                // Compare register A and 8-bit immediate n
                case 0xFE:
                    Sub8(GetA(), mem[PC + 1]);
                    PC++;
                    break;

                // CP (HL)
                // Compare register A and memory at address HL
                case 0xBE:
                    Sub8(GetA(), mem[GetHL()]);
                    break;

                // Inc r
                // Increment register r
                case 0x3C:
                    SetA(Inc(GetA()));
                    break;
                case 0x04:
                    SetB(Inc(GetB()));
                    break;
                case 0x0C:
                    SetC(Inc(GetC()));
                    break;
                case 0x14:
                    SetD(Inc(GetD()));
                    break;
                case 0x1C:
                    SetE(Inc(GetE()));
                    break;
                case 0x24:
                    SetH(Inc(GetH()));
                    break;
                case 0x2C:
                    SetL(Inc(GetL()));
                    break;

                // Inc (HL)
                // Increment memory at address HL
                case 0x34:
                    mem[GetHL()] = Inc(mem[GetHL()]);
                    break;

                // Dec r
                // Decrement register r
                case 0x3D:
                    SetA(Dec(GetA()));
                    break;
                case 0x05:
                    SetB(Dec(GetB()));
                    break;
                case 0x0D:
                    SetC(Dec(GetC()));
                    break;
                case 0x15:
                    SetD(Dec(GetD()));
                    break;
                case 0x1D:
                    SetE(Dec(GetE()));
                    break;
                case 0x25:
                    SetH(Dec(GetH()));
                    break;
                case 0x2D:
                    SetL(Dec(GetL()));
                    break;

                // Dec (HL)
                // Decrement memory at address HL
                case 0x35:
                    mem[GetHL()] = Dec(mem[GetHL()]);
                    break;



                // -------------------- 16-BIT ARITHMETIC OPERATION INSTRUCTIONS --------------------


                // ADD HL, ss
                // Add register HL and register ss and store store result in HL
                case 0x09:
                    SetHL(Add16(GetHL(), GetBC()));
                    break;
                case 0x19:
                    SetHL(Add16(GetHL(), GetDE()));
                    break;
                case 0x29:
                    SetHL(Add16(GetHL(), GetHL()));
                    break;
                case 0x39:
                    SetHL(Add16(GetHL(), SP));
                    break;

                // ADD SP, e
                // Add 8-bit immediate e to SP and store result in SP
                case 0xE8:
                    SP = Add16(SP, mem[PC + 1]);
                    PC++;
                    break;

                // Inc ss
                // Increment register ss
                case 0x03:
                    SetBC(GetBC() + 1);
                    break;
                case 0x13:
                    SetDE(GetDE() + 1);
                    break;
                case 0x23:
                    SetHL(GetHL() + 1);
                    break;
                case 0x33:
                    SP++;
                    break;

                // Dec ss
                // Decrement register ss
                case 0x0B:
                    SetBC(GetBC() - 1);
                    break;
                case 0x1B:
                    SetDE(GetDE() - 1);
                    break;
                case 0x2B:
                    SetHL(GetHL() - 1);
                    break;
                case 0x3B:
                    SP--;
                    break;



                // -------------------- ROTATE SHIFT INSTRUCTIONS --------------------

                // RLCA
                // Rotate register A to the left
                case 0x07:
                    SetA(RotateLeft(GetA()));
                    break;

                // RLA
                // Rotate register A to the left through the carry flag
                case 0x17:
                    SetA(RotateLeftCarry(GetA()));
                    break;

                // RRCA
                // Rotate register A to the right
                case 0x0F:
                    SetA(RotateRight(GetA()));
                    break;

                // RRA
                // Rotate register A to the right through the carry flag
                case 0x1F:
                    SetA(RotateRightCarry(GetA()));
                    break;


                // -------------------- CB PREFIX OPCODES --------------------
                case 0xCB:
                    switch (mem[PC + 1])
                    {

                        // RLC r
                        // Rotate register r to the left
                        case 0x07:
                            SetA(RotateLeft(GetA()));
                            break;
                        case 0x00:
                            SetB(RotateLeft(GetB()));
                            break;
                        case 0x01:
                            SetC(RotateLeft(GetC()));
                            break;
                        case 0x02:
                            SetD(RotateLeft(GetD()));
                            break;
                        case 0x03:
                            SetE(RotateLeft(GetE()));
                            break;
                        case 0x04:
                            SetH(RotateLeft(GetH()));
                            break;
                        case 0x05:
                            SetL(RotateLeft(GetL()));
                            break;

                        // RLC (HL)
                        // Rotate memory at address HL to the left
                        case 0x06:
                            mem[GetHL()] = RotateLeft(mem[GetHL()]);
                            break;

                        // RL r
                        // Rotate register r to the left through carry flag
                        case 0x17:
                            SetA(RotateLeftCarry(GetA()));
                            break;
                        case 0x10:
                            SetB(RotateLeftCarry(GetB()));
                            break;
                        case 0x11:
                            SetC(RotateLeftCarry(GetC()));
                            break;
                        case 0x12:
                            SetD(RotateLeftCarry(GetD()));
                            break;
                        case 0x13:
                            SetE(RotateLeftCarry(GetE()));
                            break;
                        case 0x14:
                            SetH(RotateLeftCarry(GetH()));
                            break;
                        case 0x15:
                            SetL(RotateLeftCarry(GetL()));
                            break;

                        // RL (HL)
                        // Rotate memory at address HL to the left through carry flag
                        case 0x16:
                            mem[GetHL()] = RotateLeftCarry(mem[GetHL()]);
                            break;

                        // RRC r
                        // Rotate register r to the right
                        case 0x0F:
                            SetA(RotateRight(GetA()));
                            break;
                        case 0x08:
                            SetB(RotateRight(GetB()));
                            break;
                        case 0x09:
                            SetC(RotateRight(GetC()));
                            break;
                        case 0x0A:
                            SetD(RotateRight(GetD()));
                            break;
                        case 0x0B:
                            SetE(RotateRight(GetE()));
                            break;
                        case 0x0C:
                            SetH(RotateRight(GetH()));
                            break;
                        case 0x0D:
                            SetL(RotateRight(GetL()));
                            break;

                        // RRC (HL)
                        // Rotate memory at address HL to the right
                        case 0x0E:
                            mem[GetHL()] = RotateLeft(mem[GetHL()]);
                            break;

                        // RR r
                        // Rotate register r to the right through carry flag
                        case 0x1F:
                            SetA(RotateRightCarry(GetA()));
                            break;
                        case 0x18:
                            SetB(RotateRightCarry(GetB()));
                            break;
                        case 0x19:
                            SetC(RotateRightCarry(GetC()));
                            break;
                        case 0x1A:
                            SetD(RotateRightCarry(GetD()));
                            break;
                        case 0x1B:
                            SetE(RotateRightCarry(GetE()));
                            break;
                        case 0x1C:
                            SetH(RotateRightCarry(GetH()));
                            break;
                        case 0x1D:
                            SetL(RotateRightCarry(GetL()));
                            break;

                        // RR (HL)
                        // Rotate memory at address HL to the left through carry flag
                        case 0x1E:
                            mem[GetHL()] = RotateRightCarry(mem[GetHL()]);
                            break;

                        // SLA r
                        // Shift register r to the left
                        case 0x27:
                            SetA(ShiftLeft(GetA()));
                            break;
                        case 0x20:
                            SetB(ShiftLeft(GetB()));
                            break;
                        case 0x21:
                            SetC(ShiftLeft(GetC()));
                            break;
                        case 0x22:
                            SetD(ShiftLeft(GetD()));
                            break;
                        case 0x23:
                            SetE(ShiftLeft(GetE()));
                            break;
                        case 0x24:
                            SetH(ShiftLeft(GetH()));
                            break;
                        case 0x25:
                            SetL(ShiftLeft(GetL()));
                            break;

                        // SLA (HL)
                        // Shift memory at register HL left
                        case 0x26:
                            mem[GetHL()] = ShiftLeft(mem[GetHL()]);
                            break;

                        // SRA r
                        // Shift register r to the right
                        case 0x2F:
                            SetA(ShiftRightArithmetic(GetA()));
                            break;
                        case 0x28:
                            SetB(ShiftRightArithmetic(GetB()));
                            break;
                        case 0x29:
                            SetC(ShiftRightArithmetic(GetC()));
                            break;
                        case 0x2A:
                            SetD(ShiftRightArithmetic(GetD()));
                            break;
                        case 0x2B:
                            SetE(ShiftRightArithmetic(GetE()));
                            break;
                        case 0x2C:
                            SetH(ShiftRightArithmetic(GetH()));
                            break;
                        case 0x2D:
                            SetL(ShiftRightArithmetic(GetL()));
                            break;

                        // SRA (HL)
                        // Shift memory at HL to the right
                        case 0x2E:
                            mem[GetHL()] = ShiftRightArithmetic(mem[GetHL()]);
                            break;

                        // SRL r
                        // Shift register r to the right logically
                        case 0x3F:
                            SetA(ShiftRightLogical(GetA()));
                            break;
                        case 0x38:
                            SetB(ShiftRightLogical(GetB()));
                            break;
                        case 0x39:
                            SetC(ShiftRightLogical(GetC()));
                            break;
                        case 0x3A:
                            SetD(ShiftRightLogical(GetD()));
                            break;
                        case 0x3B:
                            SetE(ShiftRightLogical(GetE()));
                            break;
                        case 0x3C:
                            SetH(ShiftRightLogical(GetH()));
                            break;
                        case 0x3D:
                            SetL(ShiftRightLogical(GetL()));
                            break;

                        // SRL (HL)
                        // Shift memory at HL to the right logically
                        case 0x3E:
                            mem[GetHL()] = ShiftRightLogical(mem[GetHL()]);
                            break;

                        // Swap r
                        // Swap upper and lower nibbles of register r
                        case 0x37:
                            SetA(Swap(GetA()));
                            break;
                        case 0x30:
                            SetB(Swap(GetB()));
                            break;
                        case 0x31:
                            SetC(Swap(GetC()));
                            break;
                        case 0x32:
                            SetD(Swap(GetD()));
                            break;
                        case 0x33:
                            SetE(Swap(GetE()));
                            break;
                        case 0x34:
                            SetH(Swap(GetH()));
                            break;
                        case 0x35:
                            SetL(Swap(GetL()));
                            break;

                        // Swap (HL)
                        // Swap upper and lower nibbles of memory at HL
                        case 0x36:
                            mem[GetHL()] = Swap(mem[GetHL()]);
                            break;

                        // BIT b, r
                        // Copies complement of specified bit b of register r to Z flag

                        // register A
                        case 0x47:
                            bit(GetA(), 0);
                            break;
                        case 0x4F:
                            bit(GetA(), 1);
                            break;
                        case 0x57:
                            bit(GetA(), 2);
                            break;
                        case 0x5F:
                            bit(GetA(), 3);
                            break;
                        case 0x67:
                            bit(GetA(), 4);
                            break;
                        case 0x6F:
                            bit(GetA(), 5);
                            break;
                        case 0x77:
                            bit(GetA(), 6);
                            break;
                        case 0x7F:
                            bit(GetA(), 7);
                            break;

                        // register B
                        case 0x40:
                            bit(GetB(), 0);
                            break;
                        case 0x48:
                            bit(GetB(), 1);
                            break;
                        case 0x50:
                            bit(GetB(), 2);
                            break;
                        case 0x58:
                            bit(GetB(), 3);
                            break;
                        case 0x60:
                            bit(GetB(), 4);
                            break;
                        case 0x68:
                            bit(GetB(), 5);
                            break;
                        case 0x70:
                            bit(GetB(), 6);
                            break;
                        case 0x78:
                            bit(GetB(), 7);
                            break;

                        // register C
                        case 0x41:
                            bit(GetC(), 0);
                            break;
                        case 0x49:
                            bit(GetC(), 1);
                            break;
                        case 0x51:
                            bit(GetC(), 2);
                            break;
                        case 0x59:
                            bit(GetC(), 3);
                            break;
                        case 0x61:
                            bit(GetC(), 4);
                            break;
                        case 0x69:
                            bit(GetC(), 5);
                            break;
                        case 0x71:
                            bit(GetC(), 6);
                            break;
                        case 0x79:
                            bit(GetC(), 7);
                            break;

                        // register D
                        case 0x42:
                            bit(GetD(), 0);
                            break;
                        case 0x4A:
                            bit(GetD(), 1);
                            break;
                        case 0x52:
                            bit(GetD(), 2);
                            break;
                        case 0x5A:
                            bit(GetD(), 3);
                            break;
                        case 0x62:
                            bit(GetD(), 4);
                            break;
                        case 0x6A:
                            bit(GetD(), 5);
                            break;
                        case 0x72:
                            bit(GetD(), 6);
                            break;
                        case 0x7A:
                            bit(GetD(), 7);
                            break;

                        // register E
                        case 0x43:
                            bit(GetE(), 0);
                            break;
                        case 0x4B:
                            bit(GetE(), 1);
                            break;
                        case 0x53:
                            bit(GetE(), 2);
                            break;
                        case 0x5B:
                            bit(GetE(), 3);
                            break;
                        case 0x63:
                            bit(GetE(), 4);
                            break;
                        case 0x6B:
                            bit(GetE(), 5);
                            break;
                        case 0x73:
                            bit(GetE(), 6);
                            break;
                        case 0x7B:
                            bit(GetE(), 7);
                            break;

                        // register H
                        case 0x44:
                            bit(GetH(), 0);
                            break;
                        case 0x4C:
                            bit(GetH(), 1);
                            break;
                        case 0x54:
                            bit(GetH(), 2);
                            break;
                        case 0x5C:
                            bit(GetH(), 3);
                            break;
                        case 0x64:
                            bit(GetH(), 4);
                            break;
                        case 0x6C:
                            bit(GetH(), 5);
                            break;
                        case 0x74:
                            bit(GetH(), 6);
                            break;
                        case 0x7C:
                            bit(GetH(), 7);
                            break;

                        // register L
                        case 0x45:
                            bit(GetL(), 0);
                            break;
                        case 0x4D:
                            bit(GetL(), 1);
                            break;
                        case 0x55:
                            bit(GetL(), 2);
                            break;
                        case 0x5D:
                            bit(GetL(), 3);
                            break;
                        case 0x65:
                            bit(GetL(), 4);
                            break;
                        case 0x6D:
                            bit(GetL(), 5);
                            break;
                        case 0x75:
                            bit(GetL(), 6);
                            break;
                        case 0x7D:
                            bit(GetL(), 7);
                            break;

                        // BIT b, (HL)
                        // Copies complement of specified bit b of memory at HL to Z flag
                        case 0x46:
                            bit(mem[GetHL()], 0);
                            break;
                        case 0x4E:
                            bit(mem[GetHL()], 1);
                            break;
                        case 0x56:
                            bit(mem[GetHL()], 2);
                            break;
                        case 0x5E:
                            bit(mem[GetHL()], 3);
                            break;
                        case 0x66:
                            bit(mem[GetHL()], 4);
                            break;
                        case 0x6E:
                            bit(mem[GetHL()], 5);
                            break;
                        case 0x76:
                            bit(mem[GetHL()], 6);
                            break;
                        case 0x7E:
                            bit(mem[GetHL()], 7);
                            break;

                        // SET b, r
                        // Sets bit b in register r to 1

                        // register A
                        case 0xC7:
                            SetA(set(GetA(), 0));
                            break;
                        case 0xCF:
                            SetA(set(GetA(), 1));
                            break;
                        case 0xD7:
                            SetA(set(GetA(), 2));
                            break;
                        case 0xDF:
                            SetA(set(GetA(), 3));
                            break;
                        case 0xE7:
                            SetA(set(GetA(), 4));
                            break;
                        case 0xEF:
                            SetA(set(GetA(), 5));
                            break;
                        case 0xF7:
                            SetA(set(GetA(), 6));
                            break;
                        case 0xFF:
                            SetA(set(GetA(), 7));
                            break;

                        // register B
                        case 0xC0:
                            SetB(set(GetB(), 0));
                            break;
                        case 0xC8:
                            SetB(set(GetB(), 1));
                            break;
                        case 0xD0:
                            SetB(set(GetB(), 2));
                            break;
                        case 0xD8:
                            SetB(set(GetB(), 3));
                            break;
                        case 0xE0:
                            SetB(set(GetB(), 4));
                            break;
                        case 0xE8:
                            SetB(set(GetB(), 5));
                            break;
                        case 0xF0:
                            SetB(set(GetB(), 6));
                            break;
                        case 0xF8:
                            SetB(set(GetB(), 7));
                            break;

                        // register C
                        case 0xC1:
                            SetC(set(GetC(), 0));
                            break;
                        case 0xC9:
                            SetC(set(GetC(), 1));
                            break;
                        case 0xD1:
                            SetC(set(GetC(), 2));
                            break;
                        case 0xD9:
                            SetC(set(GetC(), 3));
                            break;
                        case 0xE1:
                            SetC(set(GetC(), 4));
                            break;
                        case 0xE9:
                            SetC(set(GetC(), 5));
                            break;
                        case 0xF1:
                            SetC(set(GetC(), 6));
                            break;
                        case 0xF9:
                            SetC(set(GetC(), 7));
                            break;

                        // register D
                        case 0xC2:
                            SetD(set(GetD(), 0));
                            break;
                        case 0xCA:
                            SetD(set(GetD(), 1));
                            break;
                        case 0xD2:
                            SetD(set(GetD(), 2));
                            break;
                        case 0xDA:
                            SetD(set(GetD(), 3));
                            break;
                        case 0xE2:
                            SetD(set(GetD(), 4));
                            break;
                        case 0xEA:
                            SetD(set(GetD(), 5));
                            break;
                        case 0xF2:
                            SetD(set(GetD(), 6));
                            break;
                        case 0xFA:
                            SetD(set(GetD(), 7));
                            break;

                        // register E
                        case 0xC3:
                            SetE(set(GetE(), 0));
                            break;
                        case 0xCB:
                            SetE(set(GetE(), 1));
                            break;
                        case 0xD3:
                            SetE(set(GetE(), 2));
                            break;
                        case 0xDB:
                            SetE(set(GetE(), 3));
                            break;
                        case 0xE3:
                            SetE(set(GetE(), 4));
                            break;
                        case 0xEB:
                            SetE(set(GetE(), 5));
                            break;
                        case 0xF3:
                            SetE(set(GetE(), 6));
                            break;
                        case 0xFB:
                            SetE(set(GetE(), 7));
                            break;

                        // register H
                        case 0xC4:
                            SetH(set(GetH(), 0));
                            break;
                        case 0xCC:
                            SetH(set(GetH(), 1));
                            break;
                        case 0xD4:
                            SetH(set(GetH(), 2));
                            break;
                        case 0xDC:
                            SetH(set(GetH(), 3));
                            break;
                        case 0xE4:
                            SetH(set(GetH(), 4));
                            break;
                        case 0xEC:
                            SetH(set(GetH(), 5));
                            break;
                        case 0xF4:
                            SetH(set(GetH(), 6));
                            break;
                        case 0xFC:
                            SetH(set(GetH(), 7));
                            break;

                        // register L
                        case 0xC5:
                            SetL(set(GetL(), 0));
                            break;
                        case 0xCD:
                            SetL(set(GetL(), 1));
                            break;
                        case 0xD5:
                            SetL(set(GetL(), 2));
                            break;
                        case 0xDD:
                            SetL(set(GetL(), 3));
                            break;
                        case 0xE5:
                            SetL(set(GetL(), 4));
                            break;
                        case 0xED:
                            SetL(set(GetL(), 5));
                            break;
                        case 0xF5:
                            SetL(set(GetL(), 6));
                            break;
                        case 0xFD:
                            SetL(set(GetL(), 7));
                            break;

                        // SET b, (HL)
                        // Sets bit b in memory at HL to 1
                        case 0xC6:
                            mem[GetHL()] = set(mem[GetHL()], 0);
                            break;
                        case 0xCE:
                            mem[GetHL()] = set(mem[GetHL()], 1);
                            break;
                        case 0xD6:
                            mem[GetHL()] = set(mem[GetHL()], 2);
                            break;
                        case 0xDE:
                            mem[GetHL()] = set(mem[GetHL()], 3);
                            break;
                        case 0xE6:
                            mem[GetHL()] = set(mem[GetHL()], 4);
                            break;
                        case 0xEE:
                            mem[GetHL()] = set(mem[GetHL()], 5);
                            break;
                        case 0xF6:
                            mem[GetHL()] = set(mem[GetHL()], 6);
                            break;
                        case 0xFE:
                            mem[GetHL()] = set(mem[GetHL()], 7);
                            break;

                        // RES b, r
                        // Set bit b in register r to 0

                        // register A
                        case 0x87:
                            SetA(reset(GetA(), 0));
                            break;
                        case 0x8F:
                            SetA(reset(GetA(), 1));
                            break;
                        case 0x97:
                            SetA(reset(GetA(), 2));
                            break;
                        case 0x9F:
                            SetA(reset(GetA(), 3));
                            break;
                        case 0xA7:
                            SetA(reset(GetA(), 4));
                            break;
                        case 0xAF:
                            SetA(reset(GetA(), 5));
                            break;
                        case 0xB7:
                            SetA(reset(GetA(), 6));
                            break;
                        case 0xBF:
                            SetA(reset(GetA(), 7));
                            break;

                        // register B
                        case 0x80:
                            SetB(reset(GetB(), 0));
                            break;
                        case 0x88:
                            SetB(reset(GetB(), 1));
                            break;
                        case 0x90:
                            SetB(reset(GetB(), 2));
                            break;
                        case 0x98:
                            SetB(reset(GetB(), 3));
                            break;
                        case 0xA0:
                            SetB(reset(GetB(), 4));
                            break;
                        case 0xA8:
                            SetB(reset(GetB(), 5));
                            break;
                        case 0xB0:
                            SetB(reset(GetB(), 6));
                            break;
                        case 0xB8:
                            SetB(reset(GetB(), 7));
                            break;

                        // register C
                        case 0x81:
                            SetC(reset(GetC(), 0));
                            break;
                        case 0x89:
                            SetC(reset(GetC(), 1));
                            break;
                        case 0x91:
                            SetC(reset(GetC(), 2));
                            break;
                        case 0x99:
                            SetC(reset(GetC(), 3));
                            break;
                        case 0xA1:
                            SetC(reset(GetC(), 4));
                            break;
                        case 0xA9:
                            SetC(reset(GetC(), 5));
                            break;
                        case 0xB1:
                            SetC(reset(GetC(), 6));
                            break;
                        case 0xB9:
                            SetC(reset(GetC(), 7));
                            break;

                        // register D
                        case 0x82:
                            SetD(reset(GetD(), 0));
                            break;
                        case 0x8A:
                            SetD(reset(GetD(), 1));
                            break;
                        case 0x92:
                            SetD(reset(GetD(), 2));
                            break;
                        case 0x9A:
                            SetD(reset(GetD(), 3));
                            break;
                        case 0xA2:
                            SetD(reset(GetD(), 4));
                            break;
                        case 0xAA:
                            SetD(reset(GetD(), 5));
                            break;
                        case 0xB2:
                            SetD(reset(GetD(), 6));
                            break;
                        case 0xBA:
                            SetD(reset(GetD(), 7));
                            break;

                        // register E
                        case 0x83:
                            SetE(reset(GetE(), 0));
                            break;
                        case 0x8B:
                            SetE(reset(GetE(), 1));
                            break;
                        case 0x93:
                            SetE(reset(GetE(), 2));
                            break;
                        case 0x9B:
                            SetE(reset(GetE(), 3));
                            break;
                        case 0xA3:
                            SetE(reset(GetE(), 4));
                            break;
                        case 0xAB:
                            SetE(reset(GetE(), 5));
                            break;
                        case 0xB3:
                            SetE(reset(GetE(), 6));
                            break;
                        case 0xBB:
                            SetE(reset(GetE(), 7));
                            break;

                        // register H
                        case 0x84:
                            SetH(reset(GetH(), 0));
                            break;
                        case 0x8C:
                            SetH(reset(GetH(), 1));
                            break;
                        case 0x94:
                            SetH(reset(GetH(), 2));
                            break;
                        case 0x9C:
                            SetH(reset(GetH(), 3));
                            break;
                        case 0xA4:
                            SetH(reset(GetH(), 4));
                            break;
                        case 0xAC:
                            SetH(reset(GetH(), 5));
                            break;
                        case 0xB4:
                            SetH(reset(GetH(), 6));
                            break;
                        case 0xBC:
                            SetH(reset(GetH(), 7));
                            break;

                        // register L
                        case 0x85:
                            SetL(reset(GetL(), 0));
                            break;
                        case 0x8D:
                            SetL(reset(GetL(), 1));
                            break;
                        case 0x95:
                            SetL(reset(GetL(), 2));
                            break;
                        case 0x9D:
                            SetL(reset(GetL(), 3));
                            break;
                        case 0xA5:
                            SetL(reset(GetL(), 4));
                            break;
                        case 0xAD:
                            SetL(reset(GetL(), 5));
                            break;
                        case 0xB5:
                            SetL(reset(GetL(), 6));
                            break;
                        case 0xBD:
                            SetL(reset(GetL(), 7));
                            break;

                        // RES b, (HL)
                        // Reset bit b in memory at HL to 0
                        case 0x86:
                            mem[GetHL()] = reset(mem[GetHL()], 0);
                            break;
                        case 0x8E:
                            mem[GetHL()] = reset(mem[GetHL()], 1);
                            break;
                        case 0x96:
                            mem[GetHL()] = reset(mem[GetHL()], 2);
                            break;
                        case 0x9E:
                            mem[GetHL()] = reset(mem[GetHL()], 3);
                            break;
                        case 0xA6:
                            mem[GetHL()] = reset(mem[GetHL()], 4);
                            break;
                        case 0xAE:
                            mem[GetHL()] = reset(mem[GetHL()], 5);
                            break;
                        case 0xB6:
                            mem[GetHL()] = reset(mem[GetHL()], 6);
                            break;
                        case 0xBE:
                            mem[GetHL()] = reset(mem[GetHL()], 7);
                            break;
                    }
                    PC++;
                    break;

                // JP nn
                // Jump to address specified by 16 bit immediate
                case 0xC3:
                    PC = (mem[PC + 2] << 8) | mem[PC + 1];
                    PC--;
                    break;

                // JP cc, nn
                // Jump to address specific if condition cc is met

                // Z == 0
                case 0xC2:
                    if (!GetZero())
                    {
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                    }
                    break;

                // Z == 1
                case 0xCA:
                    if (GetZero())
                    {
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                    }
                    break;

                // C == 0
                case 0xD2:
                    if (!GetCarry())
                    {
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                    }
                    break;

                // C == 1
                case 0xDA:
                    if (GetCarry())
                    {
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                    }
                    break;

                // JR e
                // Jump to PC + (e - 2)
                case 0x18:
                    PC += (int8_t)mem[PC + 1];
                    PC++;
                    break;

                // JR cc, e
                // Jump to PC + (e - 2) if condition cc is met

                // Z == 0
                case 0x20:
                    if (!GetZero())
                    {
                        PC += (int8_t)mem[PC + 1];
                        PC++;
                    }
                    break;

                // Z == 1
                case 0x28:
                    if (GetZero())
                    {
                        PC += (int8_t)mem[PC + 1];
                        PC++;
                    }
                    break;

                // C == 0
                case 0x30:
                    if (!GetCarry())
                    {
                        PC += (int8_t)mem[PC + 1];
                        PC++;
                    }
                    break;

                // C == 1
                case 0x38:
                    if (!GetCarry())
                    {
                        PC += (int8_t)mem[PC + 1];
                        PC++;
                    }
                    break;

                // JP (HL)
                // Jump to address specified by register HL
                case 0xE9:
                    PC = GetHL();
                    PC--;
                    break;

                // CALL nn
                // Push current address onto stack and jump to address specified by 16 bit immediate
                case 0xCD:
                    mem[SP - 1] = (PC >> 8) & 0xFF;
                    mem[SP - 2] = PC & 0xFF;
                    PC = (mem[PC + 2] << 8) | mem[PC + 1];
                    PC--;
                    SP -= 2;
                    break;

                // CALL cc, nn
                // Call address at nn if condition cc is met

                // Z == 0
                case 0xC4:
                    if (!GetZero())
                    {
                        mem[SP - 1] = (PC >> 8) & 0xFF;
                        mem[SP - 2] = PC & 0xFF;
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                        SP -= 2;
                    }
                    break;

                // Z == 1
                case 0xCC:
                    if (GetZero())
                    {
                        mem[SP - 1] = (PC >> 8) & 0xFF;
                        mem[SP - 2] = PC & 0xFF;
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                        SP -= 2;
                    }
                    break;

                // C == 0
                case 0xD4:
                    if (!GetCarry())
                    {
                        mem[SP - 1] = (PC >> 8) & 0xFF;
                        mem[SP - 2] = PC & 0xFF;
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                        SP -= 2;
                    }
                    break;

                // C == 1
                case 0xDC:
                    if (GetCarry())
                    {
                        mem[SP - 1] = (PC >> 8) & 0xFF;
                        mem[SP - 2] = PC & 0xFF;
                        PC = (mem[PC + 2] << 8) | mem[PC + 1];
                        PC--;
                        SP -= 2;
                    }
                    break;

                // RET
                // Pop stack and return to popped address
                case 0xC9:
                    PC = (mem[SP + 1] << 8) | mem[SP];
                    SP += 2;
                    PC--;
                    break;

                // RETI
                // Return from interrupt routine
                case 0xD9:
                    PC = (mem[SP + 1] << 8) | mem[SP];
                    SP += 2;
                    PC--;
                    IME = true;
                    break;

                // RET cc
                // Return from subroutine if condition cc is true

                // Z == 0
                case 0xC0:
                    if (!GetZero())
                    {
                        PC = (mem[SP + 1] << 8) | mem[SP];
                        SP += 2;
                        PC--;
                    }
                    break;

                // Z == 1
                case 0xC8:
                    if (GetZero())
                    {
                        PC = (mem[SP + 1] << 8) | mem[SP];
                        SP += 2;
                        PC--;
                    }
                    break;

                // C == 0
                case 0xD0:
                    if (!GetCarry())
                    {
                        PC = (mem[SP + 1] << 8) | mem[SP];
                        SP += 2;
                        PC--;
                    }
                    break;

                // C == 1
                case 0xD8:
                    if (GetCarry())
                    {
                        PC = (mem[SP + 1] << 8) | mem[SP];
                        SP += 2;
                        PC--;
                    }
                    break;

                // RST t
                // Load zero page memory address specified by t into PC
                case 0xC7:
                    PC = 0x00;
                    PC--;
                    break;
                case 0xCF:
                    PC = 0x08;
                    PC--;
                    break;
                case 0xD7:
                    PC = 0x10;
                    PC--;
                    break;
                case 0xDF:
                    PC = 0x18;
                    PC--;
                    break;
                case 0xE7:
                    PC = 0x20;
                    PC--;
                    break;
                case 0xEF:
                    PC = 0x28;
                    PC--;
                    break;
                case 0xF7:
                    PC = 0x30;
                    PC--;
                    break;
                case 0xFF:
                    PC = 0x38;
                    PC--;
                    break;

                // DAA
                // Decimal adjust acc
                // TODO: implement decimal adjust instruction
                case 0x27:
                    break;

                // CPL
                // Complement register A
                case 0x2F:
                    SetA(~GetA());
                    break;

                // NOP
                // No operation
                case 0x00:
                    break;

                // CCF
                // Complement carry flag
                case 0x3F:
                    SetCarry(!GetCarry());
                    break;

                // SCF
                // Set carry flag
                case 0x37:
                    SetCarry(true);
                    break;

                // DI
                // Reset interrupt master enable flag
                case 0xF3:
                    IME = false;
                    break;

                // EI
                // Set interrupt master enable flag
                case 0xFB:
                    IME = true;
                    break;

                // HALT
                // stop system clock
                // TODO: implement halt instruction
                case 0x76:
                    break;

                // STOP
                // Stop system clock and oscillator circuit
                // TODO: implement stop instruction
                case 0x10:
                    break;
            }
            PC++;
        }

        // Add opertaion for 8-bit numbers
        // Input: a (byte), b (byte)
        // Output: a + b (byte)
        byte Add8(byte a, byte b)
        {
            ushort sum = (ushort)(a + b);
            byte half = (byte)((a & 0xF) + (b & 0xF));
            SetZero(sum == 0);
            SetHalfCarry(half > 0xF);
            SetSubtract(false);
            SetCarry(sum > 0xFF);
            return (byte)sum;
        }

        // Add opertaion for 16-bit numbers
        // Input: a (uint16_t), b (uint16_t)
        // Output: a + b (uint16_t)
        ushort Add16(ushort a, ushort b)
        {
            uint sum = (uint)(a + b);
            ushort half = (ushort)((a & 0xFF) + (b & 0xFF));
            SetZero(sum == 0);
            SetHalfCarry(half > 0xFF);
            SetSubtract(false);
            SetCarry(sum > 0xFFFF);
            return (ushort)sum;
        }

        // Subtract opertaion for 8-bit numbers
        // Input: a (byte), b (byte)
        // Output: a - b (byte)
        byte Sub8(byte a, byte b)
        {
            byte diff = (byte)(a - b);
            SetZero(diff == 0);
            SetHalfCarry((a & 0xF) < (b & 0xF));
            SetSubtract(true);
            SetCarry(a < b);
            return diff;
        }

        // ANDs two 8-bit numbers together
        // Input: a (byte), b (byte)
        // Output: a & b
        byte And8(byte a, byte b)
        {
            byte result = (byte)(a & b);
            SetZero(result == 0);
            SetHalfCarry(true);
            SetSubtract(false);
            SetCarry(false);
            return result;
        }

        // ORs two 8-bit numbers together
        // Input: a (byte), b (byte)
        // Output: a | b
        byte Or8(byte a, byte b)
        {
            byte result = (byte)(a | b);
            SetZero(result == 0);
            return result;
        }

        // XORs two 8-bit numbers together
        // Input: a (byte), b (byte)
        // Output: a ^ b
        byte Xor8(byte a, byte b)
        {
            byte result = (byte)(a ^ b);
            SetZero(result == 0);
            return result;
        }

        // Increments value by one
        // Input: value (byte)
        // Output: value + 1
        byte Inc(byte value)
        {
            bool carry = GetCarry();
            byte sum = Add8(value, 1);
            SetCarry(carry);
            return sum;
        }

        // Decrements value by one
        // Input: value (byte)
        // Output: value - 1
        byte Dec(byte value)
        {
            bool carry = GetCarry();
            byte diff = Sub8(value, 1);
            SetCarry(carry);
            return diff;
        }

        // Rotates value to the left
        // Input: value (byte)
        // Output: rotated value
        byte RotateLeft(byte value)
        {
            byte bit7 = (byte)((value & 0x80) >> 7);
            byte result = (byte)((value << 1) | bit7);
            SetCarry(Convert.ToBoolean(bit7));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return (byte)((value << 1) | bit7);
        }

        // Rotates value to the left through carry flag
        // Input: value (byte)
        // Output: rotated value
        byte RotateLeftCarry(byte value)
        {
            byte bit7 = (byte)((value & 0x80) >> 7);
            byte carry = Convert.ToByte(GetCarry());
            byte result = (byte)((value << 1) | carry);
            SetCarry(Convert.ToBoolean(bit7));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Rotates value to the right
        // Input: value (byte)
        // Output: rotated value
        byte RotateRight(byte value)
        {
            byte bit0 = (byte)(value & 0x1);
            byte result = (byte)((value >> 1) | (bit0 << 7));
            SetCarry(Convert.ToBoolean(bit0));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Rotates value to the right through carry flag
        // Input: value (byte)
        // Output: rotated value
        byte RotateRightCarry(byte value)
        {
            byte bit0 = (byte)(value & 0x1);
            byte carry = Convert.ToByte(GetCarry());
            byte result = (byte)((value >> 1) | (carry << 7));
            SetCarry(Convert.ToBoolean(bit0));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Shifts value to the left by one
        // Input: value (byte)
        // Output: value << 1
        byte ShiftLeft(byte value)
        {
            byte result = (byte)(value << 1);
            SetCarry(Convert.ToBoolean((value & 0x80) >> 7));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Arithmetically shifts value to the right by one
        // Input: value (byte)
        // Output: value >> 1
        byte ShiftRightArithmetic(byte value)
        {
            byte msb = (byte)(value & 0x80);
            byte result = (byte)((value >> 1) | msb);
            SetCarry(Convert.ToBoolean(value & 0x1));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Logically shifts value to the right by one
        // Input: value (byte)
        // Output: value >> 1
        byte ShiftRightLogical(byte value)
        {
            byte result = (byte)((value >> 1) & 0x7F);
            SetCarry(Convert.ToBoolean(value & 0x1));
            SetZero(result == 0);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Swaps lower and upper nibbles of specified value
        // Input: value (byte)
        // Output: swapped value (byte)
        byte Swap(byte value)
        {
            byte hi = (byte)((value >> 4) & 0xF);
            byte lo = (byte)((value << 4) & 0xF0);
            byte result = (byte)(lo | hi);
            SetZero(result == 0);
            SetCarry(false);
            SetHalfCarry(false);
            SetSubtract(false);
            return result;
        }

        // Copies complement of bit in value into zero flag
        // Input: value (byte), bit (int)
        // Output: none
        void Bit(byte value, int bit)
        {
            SetZero(Convert.ToBoolean(~(value >> bit) & 0x1));
        }

        // Sets specified bit to 1
        // Input: value (byte), bit (int)
        // Output: value with specified bit set to 1 (byte)
        byte Set(byte value, int bit)
        {
            return (byte)(value | (1 << bit));
        }

        // Sets specified bit to 0
        // Input: value (byte), bit (int)
        // Output: value with specified bit set to 0 (byte)
        byte Reset(byte value, int bit)
        {
            return (byte)(value & ~(1 << bit));
        }

        // 8-bit register setters
        void SetA(byte value) { AF = (ushort)((AF & 0xFF) | (value << 8)); }
        void SetF(byte value) { AF = (ushort)((AF & 0xFF00) | value); }
        void SetB(byte value) { BC = (ushort)((BC & 0xFF) | (value << 8)); }
        void SetC(byte value) { BC = (ushort)((BC & 0xFF00) | value); }
        void SetD(byte value) { DE = (ushort)((DE & 0xFF) | (value << 8)); }
        void SetE(byte value) { DE = (ushort)((DE & 0xFF00) | value); }
        void SetH(byte value) { HL = (ushort)((HL & 0xFF) | (value << 8)); }
        void SetL(byte value) { HL = (ushort)((HL & 0xFF00) | value); }

        // 8-bit register getters
        byte GetA() { return (byte)((AF >> 8) & 0xFF); }
        byte GetF() { return (byte)(AF & 0xFF); }
        byte GetB() { return (byte)((BC >> 8) & 0xFF); }
        byte GetC() { return (byte)(BC & 0xFF); }
        byte GetD() { return (byte)((DE >> 8) & 0xFF); }
        byte GetE() { return (byte)(DE & 0xFF); }
        byte GetH() { return (byte)((HL >> 8) & 0xFF); }
        byte GetL() { return (byte)(HL & 0xFF); }

        // 16-bit register setters
        void SetAF(ushort value) { AF = value; }
        void SetBC(ushort value) { BC = value; }
        void SetDE(ushort value) { DE = value; }
        void SetHL(ushort value) { HL = value; }

        // 16-bit register getters
        ushort GetAF() { return AF; }
        ushort GetBC() { return BC; }
        ushort GetDE() { return DE; }
        ushort GetHL() { return HL; }

        // Flag setters
        void SetZero(bool value) { AF = (byte)((AF & 0xFF7F) | (Convert.ToByte(value) << 7)); }
        void SetSubtract(bool value) { AF = (byte)((AF & 0xFFBF) | (Convert.ToByte(value) << 6)); }
        void SetHalfCarry(bool value) { AF = (byte)((AF & 0xFFDF) | (Convert.ToByte(value) << 5)); }
        void SetCarry(bool value) { AF = (byte)((AF & 0xFFEF) | (Convert.ToByte(value) << 4)); }

        // Flag getters
        bool GetZero() { return Convert.ToBoolean((AF >> 7) & 0x1); }
        bool GetSubtract() { return Convert.ToBoolean((AF >> 6) & 0x1); }
        bool GetHalfCarry() { return Convert.ToBoolean((AF >> 5) & 0x1); }
        bool GetCarry() { return Convert.ToBoolean((AF >> 4) & 0x1); }
    }
}
