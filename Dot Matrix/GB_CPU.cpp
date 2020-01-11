#include "GB_CPU.h"

// Initializes cpu data
GB_CPU::GB_CPU() {
	AF = 0;
	BC = 0;
	DE = 0;
	HL = 0;
	PC = 0;
	SP = 0xFFFE;
	IME = true;
	mem = new char[0xFFFF];
	cart = new char[0x200000];
}

// Loads cartidge data into memory
void GB_CPU::loadCartridge(string dir) {
	ifstream file(dir, ios::in | ios::binary | ios::ate);
	char* buf;
	if (file.is_open()) {
		streampos size = file.tellg();
		buf = new char[size];
		file.seekg(0, ios::beg);
		file.read(buf, size);
		file.close();
		for (long long i = 0; i < size; i++) {
			cart[i] = buf[i];
		}
	}
}

// Loads gameboy bootstrap
void GB_CPU::loadBootstrap() {
	ifstream file("C:/Users/8psco/Documents/Roms/GB/bootstrap.bin", ios::in | ios::binary | ios::ate);
	char* buf;
	if (file.is_open()) {
		streampos size = file.tellg();
		buf = new char[size];
		file.seekg(0, ios::beg);
		file.read(buf, size);
		file.close();
		for (long long i = 0; i < size; i++) {
			mem[i] = buf[i];
		}
	}
}

// Run emulator
// TODO: fully implement run function
void GB_CPU::run() {
	decode();
}

// Decode instruction
void GB_CPU::decode() {
	switch (mem[PC]) {

		// -------------------- 8-BIT TRANSFER AND INPUT/OUTPUT INSTRUCTIONS --------------------

		// LD r, r'
		// Load contents of register r' into register r

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

		// LD r, n
		// Load 8-bit immediate n into register r
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

		// LD r, (HL)
		// Load memory at address HL into register r
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

		// LD (HL), r
		// Load register r into memory at address HL
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

		// LD (HL), n
		// Load 8-bit immediate n into memory at address HL
		case 0x36:
			mem[getHL()] = mem[PC + 1];
			PC++;
			break;

		// LD A, (BC)
		// Load memory at address BC into register A
		case 0x0A:
			setA(mem[getBC()]);
			break;

		// LD A, (DE)
		// Load memory at address DE into register A
		case 0x1A:
			setA(mem[getDE()]);
			break;

		// LD a, (C)
		// Load memory at address 0xFF00 + [register C] into register A
		case 0xF2:
			setA(mem[0xFF00 + getC()]);
			break;

		// LD (C), A
		// Load register A into memory at address 0xFF00 + [register C]
		case 0xE2:
			mem[0xFF00 + getC()] = getA();
			break;

		// LD A, (n)
		// Load memory at address 0xFF00 + [8-bit immediate n] into register A
		case 0xF0:
			setA(mem[0xFF00 + mem[PC + 1]]); PC++;
			break;

		// LD (n), A
		// Load register A into memory at address 0xFF00 + [8-bit immediate n]
		case 0xE0:
			mem[0xFF00 + mem[PC + 1]] = getA(); PC++;
			break;

		// LD A, (nn)
		// Load memory at address specified by 16-bit immediate nn into register A
		case 0xFA:
			setA(mem[(mem[PC + 2] << 8) | mem[PC + 1]]); PC += 2;
			break;

		// LD (nn), A
		// Load register A into memory at address specified by 16-bit immediate nn
		case 0xEA:
			mem[(mem[PC + 2] << 8) | mem[PC + 1]] = getA(); PC += 2;
			break;

		// LD A, (HLI)
		// Load memory at address HL into register A, then increment HL
		case 0x2A:
			setA(mem[getHL()]);
			setHL(getHL() + 1);
			break;

		// LD A, (HLD)
		// Load memory at address HL into register A, then decrement HL
		case 0x3A:
			setA(mem[getHL()]);
			setHL(getHL() - 1);
			break;

		// LD (BC), A
		// Load register A into memory at address BC
		case 0x02:
			mem[getBC()] = getA();
			break;

		// LD (DE), A
		// Load register A into memory at address DE
		case 0x12:
			mem[getDE()] = getA();
			break;

		// LD (HLI), A
		// Load register A into memory at address HL, then increment HL
		case 0x22:
			mem[getHL()] = getA();
			setHL(getHL() + 1);
			break;

		// LD (HLD), A
		// Load register A into memory at address HL, then decrement HL
		case 0x32:
			mem[getHL()] = getA();
			setHL(getHL() - 1);
			break;



		// -------------------- 16-BIT TRANSFER INSTRUCTIONS --------------------

		// LD dd, nn
		// Load 2 byte immediate nn into register pair dd
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

		// LD SP, HL
		// Load register HL into SP
		case 0xF9:
			SP = getHL();
			break;

		// PUSH qq
		// Push register qq onto stack
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

		// POP qq
		// Pop contents of stack into register pair qq
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

		// LDHL SP, e
		// Add 8-bit immediate e to SP and store result in HL
		case 0xF8:
			setHL(add16(getHL(), mem[PC + 1]));
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
			setA(add(getA(), getA()));
			break;
		case 0x80:
			setA(add(getA(), getB()));
			break;
		case 0x81:
			setA(add(getA(), getC()));
			break;
		case 0x82:
			setA(add(getA(), getD()));
			break;
		case 0x83:
			setA(add(getA(), getE()));
			break;
		case 0x84:
			setA(add(getA(), getH()));
			break;
		case 0x85:
			setA(add(getA(), getL()));
			break;

		// ADD A, n
		// Add 8-bit immediate n with register A and store result in A
		case 0xC6:
			setA(add(getA(), mem[PC + 1]));
			PC++;
			break;

		// ADD A, (HL)
		// Add memory at address HL to register A and store result in A
		case 0x86:
			setA(add(getA(), mem[getHL()]));
			break;

		// ADC A, r
		// Add A, register r, and carry flag together and store result in A
		case 0x8F:
			setA(add(getA(), getA() + getCarry()));
			break;
		case 0x88:
			setA(add(getA(), getB() + getCarry()));
			break;
		case 0x89:
			setA(add(getA(), getC() + getCarry()));
			break;
		case 0x8A:
			setA(add(getA(), getD() + getCarry()));
			break;
		case 0x8B:
			setA(add(getA(), getE() + getCarry()));
			break;
		case 0x8C:
			setA(add(getA(), getH() + getCarry()));
			break;
		case 0x8D:
			setA(add(getA(), getL() + getCarry()));
			break;

		// ADC A, n
		// Add A, 8-bit immediate n, and carry flag together and store result in A
		case 0xCE:
			setA(add(getA(), mem[PC + 1] + getCarry()));
			PC++;
			break;

		// ADC A, (HL)
		// Add A, memory at address HL, and carry flag together and store result in A
		case 0x8E:
			setA(add(getA(), mem[getHL()] + getCarry()));
			break;

		// SUB r
		// Subtract register r from register A and store result in A
		case 0x97:
			setA(sub(getA(), getA()));
			break;
		case 0x90:
			setA(sub(getA(), getB()));
			break;
		case 0x91:
			setA(sub(getA(), getC()));
			break;
		case 0x92:
			setA(sub(getA(), getD()));
			break;
		case 0x93:
			setA(sub(getA(), getE()));
			break;
		case 0x94:
			setA(sub(getA(), getH()));
			break;
		case 0x95:
			setA(sub(getA(), getL()));
			break;

		// SUB n
		// Subtract 8-bit immediate n from register A and store result in A
		case 0xD6:
			setA(sub(getA(), mem[PC + 1]));
			PC++;
			break;

		// SUB (HL)
		// Subtract memory at address HL from register A and store result in 
		case 0x96:
			setA(sub(getA(), mem[getHL()]));
			break;

		// SBC A, r
		// Subtract register r and carry flag from register A and store result in A
		case 0x9F:
			setA(sub(getA(), getA() + getCarry()));
			break;
		case 0x98:
			setA(sub(getA(), getB() + getCarry()));
			break;
		case 0x99:
			setA(sub(getA(), getC() + getCarry()));
			break;
		case 0x9A:
			setA(sub(getA(), getD() + getCarry()));
			break;
		case 0x9B:
			setA(sub(getA(), getE() + getCarry()));
			break;
		case 0x9C:
			setA(sub(getA(), getH() + getCarry()));
			break;
		case 0x9D:
			setA(sub(getA(), getL() + getCarry()));
			break;

		// SBC A, n
		// Subtract 8-bit immediate n and carry flag from register A and store result in A
		case 0xDE:
			setA(sub(getA(), mem[PC + 1] + getCarry()));
			PC++;
			break;

		// SBC A, (HL)
		// Subtract memory at address HL and carry flag from register A and store result in A
		case 0x9E:
			setA(sub(getA(), mem[getHL()] + getCarry()));
			break;

		// AND r
		// And register A and register r together and store result in A
		case 0xA7:
			setA(bitAnd(getA(), getA()));
			break;
		case 0xA0:
			setA(bitAnd(getA(), getB()));
			break;
		case 0xA1:
			setA(bitAnd(getA(), getC()));
			break;
		case 0xA2:
			setA(bitAnd(getA(), getD()));
			break;
		case 0xA3:
			setA(bitAnd(getA(), getE()));
			break;
		case 0xA4:
			setA(bitAnd(getA(), getH()));
			break;
		case 0xA5:
			setA(bitAnd(getA(), getL()));
			break;

		// AND n
		// And register A and 8-bit immediate n together and store result in A
		case 0xE6:
			setA(bitAnd(getA(), mem[PC + 1]));
			PC++;
			break;

		// AND (HL)
		// And register A and memory at address HL together and store result in A
		case 0xA6:
			setA(bitAnd(getA(), mem[getHL()]));
			break;

		// OR r
		// Or register A and register r together and store result in A
		case 0xB7:
			setA(bitOr(getA(), getA()));
			break;
		case 0xB0:
			setA(bitOr(getA(), getB()));
			break;
		case 0xB1:
			setA(bitOr(getA(), getC()));
			break;
		case 0xB2:
			setA(bitOr(getA(), getD()));
			break;
		case 0xB3:
			setA(bitOr(getA(), getE()));
			break;
		case 0xB4:
			setA(bitOr(getA(), getH()));
			break;
		case 0xB5:
			setA(bitOr(getA(), getL()));
			break;

		// OR n
		// Or register A and 8-bit immediate n together and store result in A
		case 0xF6:
			setA(bitOr(getA(), mem[PC + 1]));
			PC++;
			break;

		// OR (HL)
		// Or register A and memory at address HL together and store result in A
		case 0xB6:
			setA(bitOr(getA(), mem[getHL()]));
			break;

		// XOR r
		// Xor register A and register r together and store result in A
		case 0xAF:
			setA(bitXor(getA(), getA()));
			break;
		case 0xA8:
			setA(bitXor(getA(), getB()));
			break;
		case 0xA9:
			setA(bitXor(getA(), getC()));
			break;
		case 0xAA:
			setA(bitXor(getA(), getD()));
			break;
		case 0xAB:
			setA(bitXor(getA(), getE()));
			break;
		case 0xAC:
			setA(bitXor(getA(), getH()));
			break;
		case 0xAD:
			setA(bitXor(getA(), getL()));
			break;

		// XOR n
		// Xor register A and 8-bit immediate n together and store result in A
		case 0xEE:
			setA(bitXor(getA(), mem[PC + 1]));
			PC++;
			break;

		// XOR (HL)
		// Xor register A and memory at address HL together and store result in A
		case 0xAE:
			setA(bitXor(getA(), mem[getHL()]));
			break;

		// CP r
		// Compare register A and register R
		case 0xBF:
			sub(getA(), getA());
			break;
		case 0xB8:
			sub(getA(), getB());
			break;
		case 0xB9:
			sub(getA(), getC());
			break;
		case 0xBA:
			sub(getA(), getD());
			break;
		case 0xBB:
			sub(getA(), getE());
			break;
		case 0xBC:
			sub(getA(), getH());
			break;
		case 0xBD:
			sub(getA(), getL());
			break;

		// CP n
		// Compare register A and 8-bit immediate n
		case 0xFE:
			sub(getA(), mem[PC + 1]);
			PC++;
			break;

		// CP (HL)
		// Compare register A and memory at address HL
		case 0xBE:
			sub(getA(), mem[getHL()]);
			break;

		// INC r
		// Increment register r
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

		// INC (HL)
		// Increment memory at address HL
		case 0x34:
			mem[getHL()] = inc(mem[getHL()]);
			break;

		// DEC r
		// Decrement register r
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

		// DEC (HL)
		// Decrement memory at address HL
		case 0x35:
			mem[getHL()] = dec(mem[getHL()]);
			break;



		// -------------------- 16-BIT ARITHMETIC OPERATION INSTRUCTIONS --------------------


		// ADD HL, ss
		// Add register HL and register ss and store store result in HL
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

		// ADD SP, e
		// Add 8-bit immediate e to SP and store result in SP
		case 0xE8:
			SP = add16(SP, mem[PC + 1]);
			PC++;
			break;

		// INC ss
		// Increment register ss
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

		// DEC ss
		// Decrement register ss
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



		// -------------------- ROTATE SHIFT INSTRUCTIONS --------------------

		// RLCA
		// Rotate register A to the left
		case 0x07:
			setA(rotateLeft(getA()));
			break;

		// RLA
		// Rotate register A to the left through the carry flag
		case 0x17:
			setA(rotateLeftCarry(getA()));
			break;

		// RRCA
		// Rotate register A to the right
		case 0x0F:
			setA(rotateRight(getA()));
			break;

		// RRA
		// Rotate register A to the right through the carry flag
		case 0x1F:
			setA(rotateRightCarry(getA()));
			break;


		// -------------------- CB PREFIX OPCODES --------------------
		case 0xCB:
			switch (mem[PC + 1]) {

				// RLC r
				// Rotate register r to the left
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

				// RLC (HL)
				// Rotate memory at address HL to the left
				case 0x06:
					mem[getHL()] = rotateLeft(mem[getHL()]);
					break;

				// RL r
				// Rotate register r to the left through carry flag
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

				// RL (HL)
				// Rotate memory at address HL to the left through carry flag
				case 0x16:
					mem[getHL()] = rotateLeftCarry(mem[getHL()]);
					break;

				// RRC r
				// Rotate register r to the right
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

				// RRC (HL)
				// Rotate memory at address HL to the right
				case 0x0E:
					mem[getHL()] = rotateLeft(mem[getHL()]);
					break;

				// RR r
				// Rotate register r to the right through carry flag
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

				// RR (HL)
				// Rotate memory at address HL to the left through carry flag
				case 0x1E:
					mem[getHL()] = rotateRightCarry(mem[getHL()]);
					break;

				// SLA r
				// Shift register r to the left
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

				// SLA (HL)
				// Shift memory at register HL left
				case 0x26:
					mem[getHL()] = shiftLeft(mem[getHL()]);
					break;

				// SRA r
				// Shift register r to the right
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

				// SRA (HL)
				// Shift memory at HL to the right
				case 0x2E:
					mem[getHL()] = shiftRightArithmetic(mem[getHL()]);
					break;

				// SRL r
				// Shift register r to the right logically
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

				// SRL (HL)
				// Shift memory at HL to the right logically
				case 0x3E:
					mem[getHL()] = shiftRightLogical(mem[getHL()]);
					break;

				// SWAP r
				// Swap upper and lower nibbles of register r
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

				// SWAP (HL)
				// Swap upper and lower nibbles of memory at HL
				case 0x36:
					mem[getHL()] = swap(mem[getHL()]);
					break;

				// BIT b, r
				// Copies complement of specified bit b of register r to Z flag

				// register A
				case 0x47:
					compBitToZero(getA(), 0);
					break;
				case 0x4F:
					compBitToZero(getA(), 1);
					break;
				case 0x57:
					compBitToZero(getA(), 2);
					break;
				case 0x5F:
					compBitToZero(getA(), 3);
					break;
				case 0x67:
					compBitToZero(getA(), 4);
					break;
				case 0x6F:
					compBitToZero(getA(), 5);
					break;
				case 0x77:
					compBitToZero(getA(), 6);
					break;
				case 0x7F:
					compBitToZero(getA(), 7);
					break;

				// register B
				case 0x40:
					compBitToZero(getB(), 0);
					break;
				case 0x48:
					compBitToZero(getB(), 1);
					break;
				case 0x50:
					compBitToZero(getB(), 2);
					break;
				case 0x58:
					compBitToZero(getB(), 3);
					break;
				case 0x60:
					compBitToZero(getB(), 4);
					break;
				case 0x68:
					compBitToZero(getB(), 5);
					break;
				case 0x70:
					compBitToZero(getB(), 6);
					break;
				case 0x78:
					compBitToZero(getB(), 7);
					break;

				// register C
				case 0x41:
					compBitToZero(getC(), 0);
					break;
				case 0x49:
					compBitToZero(getC(), 1);
					break;
				case 0x51:
					compBitToZero(getC(), 2);
					break;
				case 0x59:
					compBitToZero(getC(), 3);
					break;
				case 0x61:
					compBitToZero(getC(), 4);
					break;
				case 0x69:
					compBitToZero(getC(), 5);
					break;
				case 0x71:
					compBitToZero(getC(), 6);
					break;
				case 0x79:
					compBitToZero(getC(), 7);
					break;

				// register D
				case 0x42:
					compBitToZero(getD(), 0);
					break;
				case 0x4A:
					compBitToZero(getD(), 1);
					break;
				case 0x52:
					compBitToZero(getD(), 2);
					break;
				case 0x5A:
					compBitToZero(getD(), 3);
					break;
				case 0x62:
					compBitToZero(getD(), 4);
					break;
				case 0x6A:
					compBitToZero(getD(), 5);
					break;
				case 0x72:
					compBitToZero(getD(), 6);
					break;
				case 0x7A:
					compBitToZero(getD(), 7);
					break;

				// register E
				case 0x43:
					compBitToZero(getE(), 0);
					break;
				case 0x4B:
					compBitToZero(getE(), 1);
					break;
				case 0x53:
					compBitToZero(getE(), 2);
					break;
				case 0x5B:
					compBitToZero(getE(), 3);
					break;
				case 0x63:
					compBitToZero(getE(), 4);
					break;
				case 0x6B:
					compBitToZero(getE(), 5);
					break;
				case 0x73:
					compBitToZero(getE(), 6);
					break;
				case 0x7B:
					compBitToZero(getE(), 7);
					break;

				// register H
				case 0x44:
					compBitToZero(getH(), 0);
					break;
				case 0x4C:
					compBitToZero(getH(), 1);
					break;
				case 0x54:
					compBitToZero(getH(), 2);
					break;
				case 0x5C:
					compBitToZero(getH(), 3);
					break;
				case 0x64:
					compBitToZero(getH(), 4);
					break;
				case 0x6C:
					compBitToZero(getH(), 5);
					break;
				case 0x74:
					compBitToZero(getH(), 6);
					break;
				case 0x7C:
					compBitToZero(getH(), 7);
					break;

				// register L
				case 0x45:
					compBitToZero(getL(), 0);
					break;
				case 0x4D:
					compBitToZero(getL(), 1);
					break;
				case 0x55:
					compBitToZero(getL(), 2);
					break;
				case 0x5D:
					compBitToZero(getL(), 3);
					break;
				case 0x65:
					compBitToZero(getL(), 4);
					break;
				case 0x6D:
					compBitToZero(getL(), 5);
					break;
				case 0x75:
					compBitToZero(getL(), 6);
					break;
				case 0x7D:
					compBitToZero(getL(), 7);
					break;

				// BIT b, (HL)
				// Copies complement of specified bit b of memory at HL to Z flag
				case 0x46:
					compBitToZero(mem[getHL()], 0);
					break;
				case 0x4E:
					compBitToZero(mem[getHL()], 1);
					break;
				case 0x56:
					compBitToZero(mem[getHL()], 2);
					break;
				case 0x5E:
					compBitToZero(mem[getHL()], 3);
					break;
				case 0x66:
					compBitToZero(mem[getHL()], 4);
					break;
				case 0x6E:
					compBitToZero(mem[getHL()], 5);
					break;
				case 0x76:
					compBitToZero(mem[getHL()], 6);
					break;
				case 0x7E:
					compBitToZero(mem[getHL()], 7);
					break;

				// SET b, r
				// Sets bit b in register r to 1

				// register A
				case 0xC7:
					setA(setBit(getA(), 0));
					break;
				case 0xCF:
					setA(setBit(getA(), 1));
					break;
				case 0xD7:
					setA(setBit(getA(), 2));
					break;
				case 0xDF:
					setA(setBit(getA(), 3));
					break;
				case 0xE7:
					setA(setBit(getA(), 4));
					break;
				case 0xEF:
					setA(setBit(getA(), 5));
					break;
				case 0xF7:
					setA(setBit(getA(), 6));
					break;
				case 0xFF:
					setA(setBit(getA(), 7));
					break;

				// register B
				case 0xC0:
					setB(setBit(getB(), 0));
					break;
				case 0xC8:
					setB(setBit(getB(), 1));
					break;
				case 0xD0:
					setB(setBit(getB(), 2));
					break;
				case 0xD8:
					setB(setBit(getB(), 3));
					break;
				case 0xE0:
					setB(setBit(getB(), 4));
					break;
				case 0xE8:
					setB(setBit(getB(), 5));
					break;
				case 0xF0:
					setB(setBit(getB(), 6));
					break;
				case 0xF8:
					setB(setBit(getB(), 7));
					break;

				// register C
				case 0xC1:
					setC(setBit(getC(), 0));
					break;
				case 0xC9:
					setC(setBit(getC(), 1));
					break;
				case 0xD1:
					setC(setBit(getC(), 2));
					break;
				case 0xD9:
					setC(setBit(getC(), 3));
					break;
				case 0xE1:
					setC(setBit(getC(), 4));
					break;
				case 0xE9:
					setC(setBit(getC(), 5));
					break;
				case 0xF1:
					setC(setBit(getC(), 6));
					break;
				case 0xF9:
					setC(setBit(getC(), 7));
					break;

				// register D
				case 0xC2:
					setD(setBit(getD(), 0));
					break;
				case 0xCA:
					setD(setBit(getD(), 1));
					break;
				case 0xD2:
					setD(setBit(getD(), 2));
					break;
				case 0xDA:
					setD(setBit(getD(), 3));
					break;
				case 0xE2:
					setD(setBit(getD(), 4));
					break;
				case 0xEA:
					setD(setBit(getD(), 5));
					break;
				case 0xF2:
					setD(setBit(getD(), 6));
					break;
				case 0xFA:
					setD(setBit(getD(), 7));
					break;

				// register E
				case 0xC3:
					setE(setBit(getE(), 0));
					break;
				case 0xCB:
					setE(setBit(getE(), 1));
					break;
				case 0xD3:
					setE(setBit(getE(), 2));
					break;
				case 0xDB:
					setE(setBit(getE(), 3));
					break;
				case 0xE3:
					setE(setBit(getE(), 4));
					break;
				case 0xEB:
					setE(setBit(getE(), 5));
					break;
				case 0xF3:
					setE(setBit(getE(), 6));
					break;
				case 0xFB:
					setE(setBit(getE(), 7));
					break;

				// register H
				case 0xC4:
					setH(setBit(getH(), 0));
					break;
				case 0xCC:
					setH(setBit(getH(), 1));
					break;
				case 0xD4:
					setH(setBit(getH(), 2));
					break;
				case 0xDC:
					setH(setBit(getH(), 3));
					break;
				case 0xE4:
					setH(setBit(getH(), 4));
					break;
				case 0xEC:
					setH(setBit(getH(), 5));
					break;
				case 0xF4:
					setH(setBit(getH(), 6));
					break;
				case 0xFC:
					setH(setBit(getH(), 7));
					break;

				// register L
				case 0xC5:
					setL(setBit(getL(), 0));
					break;
				case 0xCD:
					setL(setBit(getL(), 1));
					break;
				case 0xD5:
					setL(setBit(getL(), 2));
					break;
				case 0xDD:
					setL(setBit(getL(), 3));
					break;
				case 0xE5:
					setL(setBit(getL(), 4));
					break;
				case 0xED:
					setL(setBit(getL(), 5));
					break;
				case 0xF5:
					setL(setBit(getL(), 6));
					break;
				case 0xFD:
					setL(setBit(getL(), 7));
					break;

				// SET b, (HL)
				// Sets bit b in memory at HL to 1
				case 0xC6:
					mem[getHL()] = setBit(mem[getHL()], 0);
					break;
				case 0xCE:
					mem[getHL()] = setBit(mem[getHL()], 1);
					break;
				case 0xD6:
					mem[getHL()] = setBit(mem[getHL()], 2);
					break;
				case 0xDE:
					mem[getHL()] = setBit(mem[getHL()], 3);
					break;
				case 0xE6:
					mem[getHL()] = setBit(mem[getHL()], 4);
					break;
				case 0xEE:
					mem[getHL()] = setBit(mem[getHL()], 5);
					break;
				case 0xF6:
					mem[getHL()] = setBit(mem[getHL()], 6);
					break;
				case 0xFE:
					mem[getHL()] = setBit(mem[getHL()], 7);
					break;

				// RES b, r
				// Set bit b in register r to 0

				// register A
				case 0x87:
					setA(resetBit(getA(), 0));
					break;
				case 0x8F:
					setA(resetBit(getA(), 1));
					break;
				case 0x97:
					setA(resetBit(getA(), 2));
					break;
				case 0x9F:
					setA(resetBit(getA(), 3));
					break;
				case 0xA7:
					setA(resetBit(getA(), 4));
					break;
				case 0xAF:
					setA(resetBit(getA(), 5));
					break;
				case 0xB7:
					setA(resetBit(getA(), 6));
					break;
				case 0xBF:
					setA(resetBit(getA(), 7));
					break;

				// register B
				case 0x80:
					setB(resetBit(getB(), 0));
					break;
				case 0x88:
					setB(resetBit(getB(), 1));
					break;
				case 0x90:
					setB(resetBit(getB(), 2));
					break;
				case 0x98:
					setB(resetBit(getB(), 3));
					break;
				case 0xA0:
					setB(resetBit(getB(), 4));
					break;
				case 0xA8:
					setB(resetBit(getB(), 5));
					break;
				case 0xB0:
					setB(resetBit(getB(), 6));
					break;
				case 0xB8:
					setB(resetBit(getB(), 7));
					break;

				// register C
				case 0x81:
					setC(resetBit(getC(), 0));
					break;
				case 0x89:
					setC(resetBit(getC(), 1));
					break;
				case 0x91:
					setC(resetBit(getC(), 2));
					break;
				case 0x99:
					setC(resetBit(getC(), 3));
					break;
				case 0xA1:
					setC(resetBit(getC(), 4));
					break;
				case 0xA9:
					setC(resetBit(getC(), 5));
					break;
				case 0xB1:
					setC(resetBit(getC(), 6));
					break;
				case 0xB9:
					setC(resetBit(getC(), 7));
					break;

				// register D
				case 0x82:
					setD(resetBit(getD(), 0));
					break;
				case 0x8A:
					setD(resetBit(getD(), 1));
					break;
				case 0x92:
					setD(resetBit(getD(), 2));
					break;
				case 0x9A:
					setD(resetBit(getD(), 3));
					break;
				case 0xA2:
					setD(resetBit(getD(), 4));
					break;
				case 0xAA:
					setD(resetBit(getD(), 5));
					break;
				case 0xB2:
					setD(resetBit(getD(), 6));
					break;
				case 0xBA:
					setD(resetBit(getD(), 7));
					break;

				// register E
				case 0x83:
					setE(resetBit(getE(), 0));
					break;
				case 0x8B:
					setE(resetBit(getE(), 1));
					break;
				case 0x93:
					setE(resetBit(getE(), 2));
					break;
				case 0x9B:
					setE(resetBit(getE(), 3));
					break;
				case 0xA3:
					setE(resetBit(getE(), 4));
					break;
				case 0xAB:
					setE(resetBit(getE(), 5));
					break;
				case 0xB3:
					setE(resetBit(getE(), 6));
					break;
				case 0xBB:
					setE(resetBit(getE(), 7));
					break;

				// register H
				case 0x84:
					setH(resetBit(getH(), 0));
					break;
				case 0x8C:
					setH(resetBit(getH(), 1));
					break;
				case 0x94:
					setH(resetBit(getH(), 2));
					break;
				case 0x9C:
					setH(resetBit(getH(), 3));
					break;
				case 0xA4:
					setH(resetBit(getH(), 4));
					break;
				case 0xAC:
					setH(resetBit(getH(), 5));
					break;
				case 0xB4:
					setH(resetBit(getH(), 6));
					break;
				case 0xBC:
					setH(resetBit(getH(), 7));
					break;

				// register L
				case 0x85:
					setL(resetBit(getL(), 0));
					break;
				case 0x8D:
					setL(resetBit(getL(), 1));
					break;
				case 0x95:
					setL(resetBit(getL(), 2));
					break;
				case 0x9D:
					setL(resetBit(getL(), 3));
					break;
				case 0xA5:
					setL(resetBit(getL(), 4));
					break;
				case 0xAD:
					setL(resetBit(getL(), 5));
					break;
				case 0xB5:
					setL(resetBit(getL(), 6));
					break;
				case 0xBD:
					setL(resetBit(getL(), 7));
					break;

				// RES b, (HL)
				// Reset bit b in memory at HL to 0
				case 0x86:
					mem[getHL()] = resetBit(mem[getHL()], 0);
					break;
				case 0x8E:
					mem[getHL()] = resetBit(mem[getHL()], 1);
					break;
				case 0x96:
					mem[getHL()] = resetBit(mem[getHL()], 2);
					break;
				case 0x9E:
					mem[getHL()] = resetBit(mem[getHL()], 3);
					break;
				case 0xA6:
					mem[getHL()] = resetBit(mem[getHL()], 4);
					break;
				case 0xAE:
					mem[getHL()] = resetBit(mem[getHL()], 5);
					break;
				case 0xB6:
					mem[getHL()] = resetBit(mem[getHL()], 6);
					break;
				case 0xBE:
					mem[getHL()] = resetBit(mem[getHL()], 7);
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
			if (!getZero()) {
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
			}
			break;

		// Z == 1
		case 0xCA:
			if (getZero()) {
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
			}
			break;

		// C == 0
		case 0xD2:
			if (!getCarry()) {
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
			}
			break;

		// C == 1
		case 0xDA:
			if (getCarry()) {
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
			if (!getZero()) {
				PC += (int8_t)mem[PC + 1];
				PC++;
			}
			break;

		// Z == 1
		case 0x28:
			if (getZero()) {
				PC += (int8_t)mem[PC + 1];
				PC++;
			}
			break;

		// C == 0
		case 0x30:
			if (!getCarry()) {
				PC += (int8_t)mem[PC + 1];
				PC++;
			}
			break;

		// C == 1
		case 0x38:
			if (!getCarry()) {
				PC += (int8_t)mem[PC + 1];
				PC++;
			}
			break;

		// JP (HL)
		// Jump to address specified by register HL
		case 0xE9:
			PC = getHL();
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
			if (!getZero()) {
				mem[SP - 1] = (PC >> 8) & 0xFF;
				mem[SP - 2] = PC & 0xFF;
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
				SP -= 2;
			}
			break;

		// Z == 1
		case 0xCC:
			if (getZero()) {
				mem[SP - 1] = (PC >> 8) & 0xFF;
				mem[SP - 2] = PC & 0xFF;
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
				SP -= 2;
			}
			break;

		// C == 0
		case 0xD4:
			if (!getCarry()) {
				mem[SP - 1] = (PC >> 8) & 0xFF;
				mem[SP - 2] = PC & 0xFF;
				PC = (mem[PC + 2] << 8) | mem[PC + 1];
				PC--;
				SP -= 2;
			}
			break;

		// C == 1
		case 0xDC:
			if (getCarry()) {
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
			if (!getZero()) {
				PC = (mem[SP + 1] << 8) | mem[SP];
				SP += 2;
				PC--;
			}
			break;

		// Z == 1
		case 0xC8:
			if (getZero()) {
				PC = (mem[SP + 1] << 8) | mem[SP];
				SP += 2;
				PC--;
			}
			break;

		// C == 0
		case 0xD0:
			if (!getCarry()) {
				PC = (mem[SP + 1] << 8) | mem[SP];
				SP += 2;
				PC--;
			}
			break;

		// C == 1
		case 0xD8:
			if (getCarry()) {
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
			setA(~getA());
			break;

		// NOP
		// No operation
		case 0x00:
			break;

		// CCF
		// Complement carry flag
		case 0x3F:
			setCarry(!getCarry());
			break;

		// SCF
		// Set carry flag
		case 0x37:
			setCarry(true);
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
// Input: a (uint8_t), b (uint8_t)
// Output: a + b (uint8_t)
char GB_CPU::add(char a, char b) {
	short sum = a + b;
	char half = (a & 0xF) + (b & 0xF);
	setZero(sum == 0);
	setHalfCarry(half > 0xF);
	setSubtract(false);
	setCarry(sum > 0xFF);
	return (char)sum;
}

// Add opertaion for 16-bit numbers
// Input: a (uint16_t), b (uint16_t)
// Output: a + b (uint16_t)
short GB_CPU::add16(short a, short b) {
	int sum = a + b;
	short half = (a & 0xFF) + (b & 0xFF);
	setZero(sum == 0);
	setHalfCarry(half > 0xFF);
	setSubtract(false);
	setCarry(sum > 0xFFFF);
	return (short)sum;
}

// Subtract opertaion for 8-bit numbers
// Input: a (uint8_t), b (uint8_t)
// Output: a - b (uint8_t)
char GB_CPU::sub(char a, char b) {
	short diff = a - b;
	setZero(diff == 0);
	setHalfCarry((a & 0xF) < (b & 0xF));
	setSubtract(true);
	setCarry(a < b);
	return (char)diff;
}

// ANDs two 8-bit numbers together
// Input: a (uint8_t), b (uint8_t)
// Output: a & b
char GB_CPU::bitAnd(char a, char b) {
	char result = a & b;
	setZero(result == 0);
	setHalfCarry(true);
	setSubtract(false);
	setCarry(false);
	return result;
}

// ORs two 8-bit numbers together
// Input: a (uint8_t), b (uint8_t)
// Output: a | b
char GB_CPU::bitOr(char a, char b) {
	char result = a | b;
	setZero(result == 0);
	return result;
}

// XORs two 8-bit numbers together
// Input: a (uint8_t), b (uint8_t)
// Output: a ^ b
char GB_CPU::bitXor(char a, char b) {
	char result = a ^ b;
	setZero(result == 0);
	return result;
}

// Increments value by one
// Input: value (uint8_t)
// Output: value + 1
char GB_CPU::inc(char value) {
	bool carry = getCarry();
	char sum = add(value, 1);
	setCarry(carry);
	return sum;
}

// Decrements value by one
// Input: value (uint8_t)
// Output: value - 1
char GB_CPU::dec(char value) {
	bool carry = getCarry();
	char diff = sub(value, 1);
	setCarry(carry);
	return diff;
}

// Rotates value to the left
// Input: value (uint8_t)
// Output: rotated value
char GB_CPU::rotateLeft(char value) {
	char bit7 = (value & 0x80) >> 7;
	char result = (value << 1) | bit7;
	setCarry(bit7);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return (value << 1) | bit7;
}

// Rotates value to the left through carry flag
// Input: value (uint8_t)
// Output: rotated value
char GB_CPU::rotateLeftCarry(char value) {
	char bit7 = (value & 0x80) >> 7;
	auto carry = (char)getCarry();
	char result = (value << 1) | carry;
	setCarry(bit7);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Rotates value to the right
// Input: value (uint8_t)
// Output: rotated value
char GB_CPU::rotateRight(char value) {
	char bit0 = value & 0x1;
	char result = (value >> 1) | (bit0 << 7);
	setCarry(bit0);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Rotates value to the right through carry flag
// Input: value (uint8_t)
// Output: rotated value
char GB_CPU::rotateRightCarry(char value) {
	char bit0 = value & 0x1;
	auto carry = (uint8_t)getCarry();
	char result = (value >> 1) | (carry << 7);
	setCarry(bit0);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Shifts value to the left by one
// Input: value (uint8_t)
// Output: value << 1
char GB_CPU::shiftLeft(char value) {
	char result = value << 1;
	setCarry((value & 0x80) >> 7);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Arithmetically shifts value to the right by one
// Input: value (uint8_t)
// Output: value >> 1
char GB_CPU::shiftRightArithmetic(char value) {
	char msb = value & 0x80;
	char result = (value >> 1) | msb;
	setCarry(value & 0x1);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Logically shifts value to the right by one
// Input: value (uint8_t)
// Output: value >> 1
char GB_CPU::shiftRightLogical(char value) {
	char result = (value >> 1) & 0x7F;
	setCarry(value & 0x1);
	setZero(result == 0);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Swaps lower and upper nibbles of specified value
// Input: value (uint8_t)
// Output: swapped value (uint8_t)
char GB_CPU::swap(char value) {
	char hi = (value >> 4) & 0xF;
	char lo = (value << 4) & 0xF0;
	char result = lo | hi;
	setZero(result == 0);
	setCarry(false);
	setHalfCarry(false);
	setSubtract(false);
	return result;
}

// Copies complement of bit in value into zero flag
// Input: value (uint8_t), bit (int)
// Output: none
void GB_CPU::compBitToZero(char value, int bit) {
	setZero(~(value >> bit) & 0x1);
}

// Sets specified bit to 1
// Input: value (uint8_t), bit (int)
// Output: value with specified bit set to 1 (uint8_t)
char GB_CPU::setBit(char value, int bit) {
	return value | (1 << bit);
}

// Sets specified bit to 0
// Input: value (uint8_t), bit (int)
// Output: value with specified bit set to 0 (uint8_t)
char GB_CPU::resetBit(char value, int bit) {
	return value & ~(1 << bit);
}



// -------------------- 8-BIT REGISTER GETTERS --------------------
char GB_CPU::getA() {
	return (AF >> 8) & 0xFF; 
}

char GB_CPU::getF() {
	return AF & 0xFF; 
}

char GB_CPU::getB() {
	return (BC >> 8) & 0xFF;
}

char GB_CPU::getC() {
	return BC & 0xFF; 
}

char GB_CPU::getD() {
	return (DE >> 8) & 0xFF;
}

char GB_CPU::getE() {
	return DE & 0xFF; 
}

char GB_CPU::getH() {
	return (HL >> 8) & 0xFF;
}

char GB_CPU::getL() {
	return HL & 0xFF; 
}



// -------------------- 8-BIT REGISTER SETTERS --------------------
void GB_CPU::setA(char value) {
	AF = (AF & 0xFF) | (value << 8); 
}

void GB_CPU::setF(char value) {
	AF = (AF & 0xFF00) | value; 
}

void GB_CPU::setB(char value) {
	BC = (BC & 0xFF) | (value << 8); 
}

void GB_CPU::setC(char value) {
	BC = (BC & 0xFF00) | value;
}

void GB_CPU::setD(char value) {
	DE = (DE & 0xFF) | (value << 8);
}

void GB_CPU::setE(char value) {
	DE = (DE & 0xFF00) | value;
}

void GB_CPU::setH(char value) {
	HL = (HL & 0xFF) | (value << 8); 
}

void GB_CPU::setL(char value) {
	HL = (HL & 0xFF00) | value; 
}



// -------------------- 16-BIT REGISTER GETTERS --------------------
short GB_CPU::getAF() {
	return AF; 
}

short GB_CPU::getBC() {
	return BC; 
}

short GB_CPU::getDE() {
	return DE;
}

short GB_CPU::getHL() {
	return HL;
}



// -------------------- 16-BIT REGISTER SETTERS --------------------
void GB_CPU::setAF(short value) {
	AF = value;
}

void GB_CPU::setBC(short value) {
	BC = value; 
}

void GB_CPU::setDE(short value) {
	DE = value; 
}

void GB_CPU::setHL(short value) {
	HL = value; 
}



// -------------------- FLAG GETTERS --------------------
char GB_CPU::getZero() {
	return (AF >> 7) & 0x1; 
}

char GB_CPU::getSubtract() {
	return (AF >> 6) & 0x1; 
}

char GB_CPU::getHalfCarry() {
	return (AF >> 5) & 0x1; 
}

char GB_CPU::getCarry() {
	return (AF >> 4) & 0x1; 
}



// -------------------- FLAG SETTERS --------------------
void GB_CPU::setZero(char value) {
	AF = (AF & 0xFF7F) | (value << 7); 
}

void GB_CPU::setSubtract(char value) {
	AF = (AF & 0xFFBF) | (value << 6); 
}

void GB_CPU::setHalfCarry(char value) {
	AF = (AF & 0xFFDF) | (value << 5); 
}

void GB_CPU::setCarry(char value) {
	AF = (AF & 0xFFEF) | (value << 4); 
}
