#include "GB_CPU.h"

// Initializes cpu data
GB_CPU::GB_CPU() {
	A = B = C = D = E = H = L = PC = 0;
	zero = halfCarry = subtract = carry = false;
	IME = true;
	SP = 0xFFFE;
	mem = new unsigned char[0xFFFF];
	cartStart = new unsigned char[0xFF];

	mem[0xFF42] = 0;
	mem[0xFF43] = 0;
}

// Loads cartidge data into memory
void GB_CPU::loadCartridge(string dir) {
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
void GB_CPU::loadBootstrap() {
	char buffer[0x100];
	ifstream bootstrap ("bootstrap.bin", ios::in | ios::binary);
	bootstrap.read(buffer, 0x100);
	unsigned short index = 0;
	for (char byte : buffer) {
		mem[index] = byte;
		index++;
	}
}

// Run emulator
// TODO: fully implement run function
void GB_CPU::step() {
	if (PC == 0x100) {
		for (int index = 0; index < 0x100; index++) {
			mem[index] = cartStart[index];
		}
	}
	decode(mem[PC]);
}

// Decode instruction
void GB_CPU::decode(unsigned char opcode) {

	switch (opcode) {

		// -------------------- 8-BIT TRANSFER AND INPUT/OUTPUT INSTRUCTIONS --------------------

		// LD r, r'
		// Load contents of register r' into register r

		// destination A
		case 0x7F:
			A = A;
			break;
		case 0x78:
			A = B;
			break;
		case 0x79:
			A = C;
			break;
		case 0x7A:
			A = D;
			break;
		case 0x7B:
			A = E;
			break;
		case 0x7C:
			A = H;
			break;
		case 0x7D:
			A = L;
			break;

		// destination B
		case 0x47:
			B = A;
			break;
		case 0x40:
			B = B;
			break;
		case 0x41:
			B = C;
			break;
		case 0x42:
			B = D;
			break;
		case 0x43:
			B = E;
			break;
		case 0x44:
			B = H;
			break;
		case 0x45:
			B = L;
			break;

		// destination C
		case 0x4F:
			C = A;
			break;
		case 0x48:
			C = B;
			break;
		case 0x49:
			C = C;
			break;
		case 0x4A:
			C = D;
			break;
		case 0x4B:
			C = E;
			break;
		case 0x4C:
			C = H;
			break;
		case 0x4D:
			C = L;
			break;

		// destination D
		case 0x57:
			D = A;
			break;
		case 0x50:
			D = B;
			break;
		case 0x51:
			D = C;
			break;
		case 0x52:
			D = D;
			break;
		case 0x53:
			D = E;
			break;
		case 0x54:
			D = H;
			break;
		case 0x55:
			D = L;
			break;

		// destination E
		case 0x5F:
			E = A;
			break;
		case 0x58:
			E = B;
			break;
		case 0x59:
			E = C;
			break;
		case 0x5A:
			E = D;
			break;
		case 0x5B:
			E = E;
			break;
		case 0x5C:
			E = H;
			break;
		case 0x5D:
			E = L;
			break;

		// destination H
		case 0x67:
			H = A;
			break;
		case 0x60:
			H = B;
			break;
		case 0x61:
			H = C;
			break;
		case 0x62:
			H = D;
			break;
		case 0x63:
			H = E;
			break;
		case 0x64:
			H = H;
			break;
		case 0x65:
			H = L;
			break;

		// destination L
		case 0x6F:
			L = A;
			break;
		case 0x68:
			L = B;
			break;
		case 0x69:
			L = C;
			break;
		case 0x6A:
			L = D;
			break;
		case 0x6B:
			L = E;
			break;
		case 0x6C:
			L = H;
			break;
		case 0x6D:
			L = L;
			break;

		// LD r, n
		// Load 8-bit immediate n into register r
		case 0x3E:
			A = getImm8();
			break;
		case 0x06:
			B = getImm8();
			break;
		case 0x0E:
			C = getImm8();
			break;
		case 0x16:
			D = getImm8();
			break;
		case 0x1E:
			E = getImm8();
			break;
		case 0x26:
			H = getImm8();
			break;
		case 0x2E:
			L = getImm8();
			break;

		// LD r, (HL)
		// Load memory at address HL into register r
		case 0x7E:
			A = readMem(getHL());
			break;
		case 0x46:
			B = readMem(getHL());
			break;
		case 0x4E:
			C = readMem(getHL());
			break;
		case 0x56:
			D = readMem(getHL());
			break;
		case 0x5E:
			E = readMem(getHL());
			break;
		case 0x66:
			H = readMem(getHL());
			break;
		case 0x6E:
			L = readMem(getHL());
			break;

		// LD (HL), r
		// Load register r into memory at address HL
		case 0x77:
			writeMem(getHL(), A);
			break;
		case 0x70:
			writeMem(getHL(), B);
			break;
		case 0x71:
			writeMem(getHL(), C);
			break;
		case 0x72:
			writeMem(getHL(), D);
			break;
		case 0x73:
			writeMem(getHL(), E);
			break;
		case 0x74:
			writeMem(getHL(), H);
			break;
		case 0x75:
			writeMem(getHL(), L);
			break;

		// LD (HL), n
		// Load 8-bit immediate n into memory at address HL
		case 0x36:
			writeMem(getHL(), getImm8());
			break;

		// LD A, (BC)
		// Load memory at address BC into register A
		case 0x0A:
			A = readMem(getBC());
			break;

		// LD A, (DE)
		// Load memory at address DE into register A
		case 0x1A:
			A = readMem(getDE());
			break;

		// LD A, (C)
		// Load memory at address 0xFF00 + [register C] into register A
		case 0xF2:
			A = readMem(0xFF00 + C);
			break;

		// LD (C), A
		// Load register A into memory at address 0xFF00 + [register C]
		case 0xE2:
			writeMem(0xFF00 + C, A);
			break;

		// LD A, (n)
		// Load memory at address 0xFF00 + [8-bit immediate n] into register A
		case 0xF0:
			A = readMem(0xFF00 + getImm8());
			break;

		// LD (n), A
		// Load register A into memory at address 0xFF00 + [8-bit immediate n]
		case 0xE0:
			writeMem(0xFF00 + getImm8(), A);
			break;

		// LD A, (nn)
		// Load memory at address specified by 16-bit immediate nn into register A
		case 0xFA:
			A = readMem(getImm16());
			break;

		// LD (nn), A
		// Load register A into memory at address specified by 16-bit immediate nn
		case 0xEA:
			writeMem(getImm16(), A);
			break;

		// LD A, (HLI)
		// Load memory at address HL into register A, then increment HL
		case 0x2A:
			A = readMem(getHL());
			setHL(getHL() + 1);
			break;

		// LD A, (HLD)
		// Load memory at address HL into register A, then decrement HL
		case 0x3A:
			A = readMem(getHL());
			setHL(getHL() - 1);
			break;

		// LD (BC), A
		// Load register A into memory at address BC
		case 0x02:
			writeMem(getBC(), A);
			break;

		// LD (DE), A
		// Load register A into memory at address DE
		case 0x12:
			writeMem(getDE(), A);
			break;

		// LD (HLI), A
		// Load register A into memory at address HL, then increment HL
		case 0x22:
			writeMem(getHL(), A);
			setHL(getHL() + 1);
			break;

		// LD (HLD), A
		// Load register A into memory at address HL, then decrement HL
		case 0x32:
			writeMem(getHL(), A);
			setHL(getHL() - 1);
			break;



		// -------------------- 16-BIT TRANSFER INSTRUCTIONS --------------------

		// LD dd, nn
		// Load 2 byte immediate nn into register pair dd
		case 0x01:
			setBC(getImm16());
			break;
		case 0x11:
			setDE(getImm16());
			break;
		case 0x21:
			setHL(getImm16());
			break;
		case 0x31:
			SP = getImm16();
			break;

		// LD SP, HL
		// Load register HL into SP
		case 0xF9:
			SP = getHL();
			break;

		// PUSH qq
		// Push register qq onto stack
		case 0xC5:
			writeMem(SP--, B);
			writeMem(SP--, C);
			break;
		case 0xD5:
			writeMem(SP--, D);
			writeMem(SP--, E);
			break;
		case 0xE5:
			writeMem(SP--, H);
			writeMem(SP--, L);
			break;
		case 0xF5:
			writeMem(SP--, A);
			writeMem(SP--, getF());
			break;

		// POP qq
		// Pop contents of stack into register pair qq
		case 0xC1:
			C = readMem(++SP);
			B = readMem(++SP);
			break;
		case 0xD1:
			E = readMem(++SP);
			D = readMem(++SP);
			break;
		case 0xE1:
			L = readMem(++SP);
			H = readMem(++SP);
			break;
		case 0xF1:
			setF(readMem(++SP));
			A = readMem(++SP);
			break;

		// LDHL SP, e
		// Add 8-bit immediate e to SP and store result in HL
		case 0xF8:
			setHL(add16(SP, getImm8()));
			zero = false;
			break;

		// LD (nn), SP
		// Load stack pointer into memory at address specified by 16-bit immediate nn
		case 0x08:
			writeMem(getImm16(), SP);
			break;



		// -------------------- 8-BIT ARITHMETIC AND LOGICAL OPERATION INSTRUCTIONS --------------------

		// ADD A, r
		// Add register r to register A and store result in A
		case 0x87:
			A = add(A, A);
			break;
		case 0x80:
			A = add(A, B);
			break;
		case 0x81:
			A = add(A, C);
			break;
		case 0x82:
			A = add(A, D);
			break;
		case 0x83:
			A = add(A, E);
			break;
		case 0x84:
			A = add(A, H);
			break;
		case 0x85:
			A = add(A, L);
			break;

		// ADD A, n
		// Add 8-bit immediate n with register A and store result in A
		case 0xC6:
			A = add(A, getImm8());
			break;

		// ADD A, (HL)
		// Add memory at address HL to register A and store result in A
		case 0x86:
			A = add(A, readMem(getHL()));
			break;

		// ADC A, r
		// Add A, register r, and carry flag together and store result in A
		case 0x8F:
			A = addCarry(A, A);
			break;
		case 0x88:
			A = addCarry(A, B);
			break;
		case 0x89:
			A = addCarry(A, C);
			break;
		case 0x8A:
			A = addCarry(A, D);
			break;
		case 0x8B:
			A = addCarry(A, E);
			break;
		case 0x8C:
			A = addCarry(A, H);
			break;
		case 0x8D:
			A = addCarry(A, L);
			break;

		// ADC A, n
		// Add A, 8-bit immediate n, and carry flag together and store result in A
		case 0xCE:
			A = addCarry(A, getImm8());
			break;

		// ADC A, (HL)
		// Add A, memory at address HL, and carry flag together and store result in A
		case 0x8E:
			A = addCarry(A, readMem(getHL()));
			break;

		// SUB r
		// Subtract register r from register A and store result in A
		case 0x97:
			A = sub(A, A);
			break;
		case 0x90:
			A = sub(A, B);
			break;
		case 0x91:
			A = sub(A, C);
			break;
		case 0x92:
			A = sub(A, D);
			break;
		case 0x93:
			A = sub(A, E);
			break;
		case 0x94:
			A = sub(A, H);
			break;
		case 0x95:
			A = sub(A, L);
			break;

		// SUB n
		// Subtract 8-bit immediate n from register A and store result in A
		case 0xD6:
			A = sub(A, getImm8());
			break;

		// SUB (HL)
		// Subtract memory at address HL from register A and store result in 
		case 0x96:
			A = sub(A, readMem(getHL()));
			break;

		// SBC A, r
		// Subtract register r and carry flag from register A and store result in A
		case 0x9F:
			A = sub(A, A + carry);
			break;
		case 0x98:
			A = sub(A, B + carry);
			break;
		case 0x99:
			A = sub(A, C + carry);
			break;
		case 0x9A:
			A = sub(A, D + carry);
			break;
		case 0x9B:
			A = sub(A, E + carry);
			break;
		case 0x9C:
			A = sub(A, H + carry);
			break;
		case 0x9D:
			A = sub(A, L + carry);
			break;

		// SBC A, n
		// Subtract 8-bit immediate n and carry flag from register A and store result in A
		case 0xDE:
			A = sub(A, getImm8() + carry);
			break;

		// SBC A, (HL)
		// Subtract memory at address HL and carry flag from register A and store result in A
		case 0x9E:
			A = sub(A, readMem(getHL()) + carry);
			break;

		// AND r
		// And register A and register r together and store result in A
		case 0xA7:
			A = bitAnd(A, A);
			break;
		case 0xA0:
			A = bitAnd(A, B);
			break;
		case 0xA1:
			A = bitAnd(A, C);
			break;
		case 0xA2:
			A = bitAnd(A, D);
			break;
		case 0xA3:
			A = bitAnd(A, E);
			break;
		case 0xA4:
			A = bitAnd(A, H);
			break;
		case 0xA5:
			A = bitAnd(A, L);
			break;

		// AND n
		// And register A and 8-bit immediate n together and store result in A
		case 0xE6:
			A = bitAnd(A, getImm8());
			break;

		// AND (HL)
		// And register A and memory at address HL together and store result in A
		case 0xA6:
			A = bitAnd(A, readMem(getHL()));
			break;

		// OR r
		// Or register A and register r together and store result in A
		case 0xB7:
			A = bitOr(A, A);
			break;
		case 0xB0:
			A = bitOr(A, B);
			break;
		case 0xB1:
			A = bitOr(A, C);
			break;
		case 0xB2:
			A = bitOr(A, D);
			break;
		case 0xB3:
			A = bitOr(A, E);
			break;
		case 0xB4:
			A = bitOr(A, H);
			break;
		case 0xB5:
			A = bitOr(A, L);
			break;

		// OR n
		// Or register A and 8-bit immediate n together and store result in A
		case 0xF6:
			A = bitOr(A, getImm8());
			break;

		// OR (HL)
		// Or register A and memory at address HL together and store result in A
		case 0xB6:
			A = bitOr(A, readMem(getHL()));
			break;

		// XOR r
		// Xor register A and register r together and store result in A
		case 0xAF:
			A = bitXor(A, A);
			break;
		case 0xA8:
			A = bitXor(A, B);
			break;
		case 0xA9:
			A = bitXor(A, C);
			break;
		case 0xAA:
			A = bitXor(A, D);
			break;
		case 0xAB:
			A = bitXor(A, E);
			break;
		case 0xAC:
			A = bitXor(A, H);
			break;
		case 0xAD:
			A = bitXor(A, L);
			break;

		// XOR n
		// Xor register A and 8-bit immediate n together and store result in A
		case 0xEE:
			A = bitXor(A, getImm8());
			break;

		// XOR (HL)
		// Xor register A and memory at address HL together and store result in A
		case 0xAE:
			A = bitXor(A, readMem(getHL()));
			break;

		// CP r
		// Compare register A and register R
		case 0xBF:
			sub(A, A);
			break;
		case 0xB8:
			sub(A, B);
			break;
		case 0xB9:
			sub(A, C);
			break;
		case 0xBA:
			sub(A, D);
			break;
		case 0xBB:
			sub(A, E);
			break;
		case 0xBC:
			sub(A, H);
			break;
		case 0xBD:
			sub(A, L);
			break;

		// CP n
		// Compare register A and 8-bit immediate n
		case 0xFE:
			sub(A, getImm8());
			break;

		// CP (HL)
		// Compare register A and memory at address HL
		case 0xBE:
			sub(A, readMem(getHL()));
			break;

		// INC r
		// Increment register r
		case 0x3C:
			A = inc(A);
			break;
		case 0x04:
			B = inc(B);
			break;
		case 0x0C:
			C = inc(C);
			break;
		case 0x14:
			D = inc(D);
			break;
		case 0x1C:
			E = inc(E);
			break;
		case 0x24:
			H = inc(H);
			break;
		case 0x2C:
			L = inc(L);
			break;

		// INC (HL)
		// Increment memory at address HL
		case 0x34:
			writeMem(getHL(), inc(readMem(getHL())));
			break;

		// DEC r
		// Decrement register r
		case 0x3D:
			A = dec(A);
			break;
		case 0x05:
			B = dec(B);
			break;
		case 0x0D:
			C = dec(C);
			break;
		case 0x15:
			D = dec(D);
			break;
		case 0x1D:
			E = dec(E);
			break;
		case 0x25:
			H = dec(H);
			break;
		case 0x2D:
			L = dec(L);
			break;

		// DEC (HL)
		// Decrement memory at address HL
		case 0x35:
			writeMem(getHL(), dec(readMem(getHL())));
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
			SP = add16(SP, getImm8());
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
			A = rotateLeft(A);
			break;

		// RLA
		// Rotate register A to the left through the carry flag
		case 0x17:
			A = rotateLeftCarry(A);
			break;

		// RRCA
		// Rotate register A to the right
		case 0x0F:
			A = rotateRight(A);
			break;

		// RRA
		// Rotate register A to the right through the carry flag
		case 0x1F:
			A = rotateRightCarry(A);
			break;


		// -------------------- CB PREFIX OPCODES --------------------
		case 0xCB:
			switch (getImm8()) {

				// RLC r
				// Rotate register r to the left
				case 0x07:
					A = rotateLeft(A);
					break;
				case 0x00:
					B = rotateLeft(B);
					break;
				case 0x01:
					C = rotateLeft(C);
					break;
				case 0x02:
					D = rotateLeft(D);
					break;
				case 0x03:
					E = rotateLeft(E);
					break;
				case 0x04:
					H = rotateLeft(H);
					break;
				case 0x05:
					L = rotateLeft(L);
					break;

				// RLC (HL)
				// Rotate memory at address HL to the left
				case 0x06:
					writeMem(getHL(), rotateLeft(readMem(getHL())));
					break;

				// RL r
				// Rotate register r to the left through carry flag
				case 0x17:
					A = rotateLeftCarry(A);
					break;
				case 0x10:
					B = rotateLeftCarry(B);
					break;
				case 0x11:
					C = rotateLeftCarry(C);
					break;
				case 0x12:
					D = rotateLeftCarry(D);
					break;
				case 0x13:
					E = rotateLeftCarry(E);
					break;
				case 0x14:
					H = rotateLeftCarry(H);
					break;
				case 0x15:
					L = rotateLeftCarry(L);
					break;

				// RL (HL)
				// Rotate memory at address HL to the left through carry flag
				case 0x16:
					writeMem(getHL(), rotateLeftCarry(readMem(getHL())));
					break;

				// RRC r
				// Rotate register r to the right
				case 0x0F:
					A = rotateRight(A);
					break;
				case 0x08:
					B = rotateRight(B);
					break;
				case 0x09:
					C = rotateRight(C);
					break;
				case 0x0A:
					D = rotateRight(D);
					break;
				case 0x0B:
					E = rotateRight(E);
					break;
				case 0x0C:
					H = rotateRight(H);
					break;
				case 0x0D:
					L = rotateRight(L);
					break;

				// RRC (HL)
				// Rotate memory at address HL to the right
				case 0x0E:
					writeMem(getHL(), rotateLeft(readMem(getHL())));
					break;

				// RR r
				// Rotate register r to the right through carry flag
				case 0x1F:
					A = rotateRightCarry(A);
					break;
				case 0x18:
					B = rotateRightCarry(B);
					break;
				case 0x19:
					C = rotateRightCarry(C);
					break;
				case 0x1A:
					D = rotateRightCarry(D);
					break;
				case 0x1B:
					E = rotateRightCarry(E);
					break;
				case 0x1C:
					H = rotateRightCarry(H);
					break;
				case 0x1D:
					L = rotateRightCarry(L);
					break;

				// RR (HL)
				// Rotate memory at address HL to the left through carry flag
				case 0x1E:
					writeMem(getHL(), rotateRightCarry(readMem(getHL())));
					break;

				// SLA r
				// Shift register r to the left
				case 0x27:
					A = shiftLeft(A);
					break;
				case 0x20:
					B = shiftLeft(B);
					break;
				case 0x21:
					C = shiftLeft(C);
					break;
				case 0x22:
					D = shiftLeft(D);
					break;
				case 0x23:
					E = shiftLeft(E);
					break;
				case 0x24:
					H = shiftLeft(H);
					break;
				case 0x25:
					L = shiftLeft(L);
					break;

				// SLA (HL)
				// Shift memory at register HL left
				case 0x26:
					writeMem(getHL(), shiftLeft(readMem(getHL())));
					break;

				// SRA r
				// Shift register r to the right
				case 0x2F:
					A = shiftRightArithmetic(A);
					break;
				case 0x28:
					B = shiftRightArithmetic(B);
					break;
				case 0x29:
					C = shiftRightArithmetic(C);
					break;
				case 0x2A:
					D = shiftRightArithmetic(D);
					break;
				case 0x2B:
					E = shiftRightArithmetic(E);
					break;
				case 0x2C:
					H = shiftRightArithmetic(H);
					break;
				case 0x2D:
					L = shiftRightArithmetic(L);
					break;

				// SRA (HL)
				// Shift memory at HL to the right
				case 0x2E:
					writeMem(getHL(), shiftRightArithmetic(readMem(getHL())));
					break;

				// SRL r
				// Shift register r to the right logically
				case 0x3F:
					A = shiftRightLogical(A);
					break;
				case 0x38:
					B = shiftRightLogical(B);
					break;
				case 0x39:
					C = shiftRightLogical(C);
					break;
				case 0x3A:
					D = shiftRightLogical(D);
					break;
				case 0x3B:
					E = shiftRightLogical(E);
					break;
				case 0x3C:
					H = shiftRightLogical(H);
					break;
				case 0x3D:
					L = shiftRightLogical(L);
					break;

				// SRL (HL)
				// Shift memory at HL to the right logically
				case 0x3E:
					writeMem(getHL(), shiftRightLogical(readMem(getHL())));
					break;

				// SWAP r
				// Swap upper and lower nibbles of register r
				case 0x37:
					A = swap(A);
					break;
				case 0x30:
					B = swap(B);
					break;
				case 0x31:
					C = swap(C);
					break;
				case 0x32:
					D = swap(D);
					break;
				case 0x33:
					E = swap(E);
					break;
				case 0x34:
					H = swap(H);
					break;
				case 0x35:
					L = swap(L);
					break;

				// SWAP (HL)
				// Swap upper and lower nibbles of memory at HL
				case 0x36:
					writeMem(getHL(), swap(readMem(getHL())));
					break;

				// BIT b, r
				// Copies complement of specified bit b of register r to Z flag

				// register A
				case 0x47:
					compBitToZero(A, 0);
					break;
				case 0x4F:
					compBitToZero(A, 1);
					break;
				case 0x57:
					compBitToZero(A, 2);
					break;
				case 0x5F:
					compBitToZero(A, 3);
					break;
				case 0x67:
					compBitToZero(A, 4);
					break;
				case 0x6F:
					compBitToZero(A, 5);
					break;
				case 0x77:
					compBitToZero(A, 6);
					break;
				case 0x7F:
					compBitToZero(A, 7);
					break;

				// register B
				case 0x40:
					compBitToZero(B, 0);
					break;
				case 0x48:
					compBitToZero(B, 1);
					break;
				case 0x50:
					compBitToZero(B, 2);
					break;
				case 0x58:
					compBitToZero(B, 3);
					break;
				case 0x60:
					compBitToZero(B, 4);
					break;
				case 0x68:
					compBitToZero(B, 5);
					break;
				case 0x70:
					compBitToZero(B, 6);
					break;
				case 0x78:
					compBitToZero(B, 7);
					break;

				// register C
				case 0x41:
					compBitToZero(C, 0);
					break;
				case 0x49:
					compBitToZero(C, 1);
					break;
				case 0x51:
					compBitToZero(C, 2);
					break;
				case 0x59:
					compBitToZero(C, 3);
					break;
				case 0x61:
					compBitToZero(C, 4);
					break;
				case 0x69:
					compBitToZero(C, 5);
					break;
				case 0x71:
					compBitToZero(C, 6);
					break;
				case 0x79:
					compBitToZero(C, 7);
					break;

				// register D
				case 0x42:
					compBitToZero(D, 0);
					break;
				case 0x4A:
					compBitToZero(D, 1);
					break;
				case 0x52:
					compBitToZero(D, 2);
					break;
				case 0x5A:
					compBitToZero(D, 3);
					break;
				case 0x62:
					compBitToZero(D, 4);
					break;
				case 0x6A:
					compBitToZero(D, 5);
					break;
				case 0x72:
					compBitToZero(D, 6);
					break;
				case 0x7A:
					compBitToZero(D, 7);
					break;

				// register E
				case 0x43:
					compBitToZero(E, 0);
					break;
				case 0x4B:
					compBitToZero(E, 1);
					break;
				case 0x53:
					compBitToZero(E, 2);
					break;
				case 0x5B:
					compBitToZero(E, 3);
					break;
				case 0x63:
					compBitToZero(E, 4);
					break;
				case 0x6B:
					compBitToZero(E, 5);
					break;
				case 0x73:
					compBitToZero(E, 6);
					break;
				case 0x7B:
					compBitToZero(E, 7);
					break;

				// register H
				case 0x44:
					compBitToZero(H, 0);
					break;
				case 0x4C:
					compBitToZero(H, 1);
					break;
				case 0x54:
					compBitToZero(H, 2);
					break;
				case 0x5C:
					compBitToZero(H, 3);
					break;
				case 0x64:
					compBitToZero(H, 4);
					break;
				case 0x6C:
					compBitToZero(H, 5);
					break;
				case 0x74:
					compBitToZero(H, 6);
					break;
				case 0x7C:
					compBitToZero(H, 7);
					break;

				// register L
				case 0x45:
					compBitToZero(L, 0);
					break;
				case 0x4D:
					compBitToZero(L, 1);
					break;
				case 0x55:
					compBitToZero(L, 2);
					break;
				case 0x5D:
					compBitToZero(L, 3);
					break;
				case 0x65:
					compBitToZero(L, 4);
					break;
				case 0x6D:
					compBitToZero(L, 5);
					break;
				case 0x75:
					compBitToZero(L, 6);
					break;
				case 0x7D:
					compBitToZero(L, 7);
					break;

				// BIT b, (HL)
				// Copies complement of specified bit b of memory at HL to Z flag
				case 0x46:
					compBitToZero(readMem(getHL()), 0);
					break;
				case 0x4E:
					compBitToZero(readMem(getHL()), 1);
					break;
				case 0x56:
					compBitToZero(readMem(getHL()), 2);
					break;
				case 0x5E:
					compBitToZero(readMem(getHL()), 3);
					break;
				case 0x66:
					compBitToZero(readMem(getHL()), 4);
					break;
				case 0x6E:
					compBitToZero(readMem(getHL()), 5);
					break;
				case 0x76:
					compBitToZero(readMem(getHL()), 6);
					break;
				case 0x7E:
					compBitToZero(readMem(getHL()), 7);
					break;

				// SET b, r
				// Sets bit b in register r to 1

				// register A
				case 0xC7:
					A = setBit(A, 0);
					break;
				case 0xCF:
					A = setBit(A, 1);
					break;
				case 0xD7:
					A = setBit(A, 2);
					break;
				case 0xDF:
					A = setBit(A, 3);
					break;
				case 0xE7:
					A = setBit(A, 4);
					break;
				case 0xEF:
					A = setBit(A, 5);
					break;
				case 0xF7:
					A = setBit(A, 6);
					break;
				case 0xFF:
					A = setBit(A, 7);
					break;

				// register B
				case 0xC0:
					B = setBit(B, 0);
					break;
				case 0xC8:
					B = setBit(B, 1);
					break;
				case 0xD0:
					B = setBit(B, 2);
					break;
				case 0xD8:
					B = setBit(B, 3);
					break;
				case 0xE0:
					B = setBit(B, 4);
					break;
				case 0xE8:
					B = setBit(B, 5);
					break;
				case 0xF0:
					B = setBit(B, 6);
					break;
				case 0xF8:
					B = setBit(B, 7);
					break;

				// register C
				case 0xC1:
					C = setBit(C, 0);
					break;
				case 0xC9:
					C = setBit(C, 1);
					break;
				case 0xD1:
					C = setBit(C, 2);
					break;
				case 0xD9:
					C = setBit(C, 3);
					break;
				case 0xE1:
					C = setBit(C, 4);
					break;
				case 0xE9:
					C = setBit(C, 5);
					break;
				case 0xF1:
					C = setBit(C, 6);
					break;
				case 0xF9:
					C = setBit(C, 7);
					break;

				// register D
				case 0xC2:
					D = setBit(D, 0);
					break;
				case 0xCA:
					D = setBit(D, 1);
					break;
				case 0xD2:
					D = setBit(D, 2);
					break;
				case 0xDA:
					D = setBit(D, 3);
					break;
				case 0xE2:
					D = setBit(D, 4);
					break;
				case 0xEA:
					D = setBit(D, 5);
					break;
				case 0xF2:
					D = setBit(D, 6);
					break;
				case 0xFA:
					D = setBit(D, 7);
					break;

				// register E
				case 0xC3:
					E = setBit(E, 0);
					break;
				case 0xCB:
					E = setBit(E, 1);
					break;
				case 0xD3:
					E = setBit(E, 2);
					break;
				case 0xDB:
					E = setBit(E, 3);
					break;
				case 0xE3:
					E = setBit(E, 4);
					break;
				case 0xEB:
					E = setBit(E, 5);
					break;
				case 0xF3:
					E = setBit(E, 6);
					break;
				case 0xFB:
					E = setBit(E, 7);
					break;

				// register H
				case 0xC4:
					H = setBit(H, 0);
					break;
				case 0xCC:
					H = setBit(H, 1);
					break;
				case 0xD4:
					H = setBit(H, 2);
					break;
				case 0xDC:
					H = setBit(H, 3);
					break;
				case 0xE4:
					H = setBit(H, 4);
					break;
				case 0xEC:
					H = setBit(H, 5);
					break;
				case 0xF4:
					H = setBit(H, 6);
					break;
				case 0xFC:
					H = setBit(H, 7);
					break;

				// register L
				case 0xC5:
					L = setBit(L, 0);
					break;
				case 0xCD:
					L = setBit(L, 1);
					break;
				case 0xD5:
					L = setBit(L, 2);
					break;
				case 0xDD:
					L = setBit(L, 3);
					break;
				case 0xE5:
					L = setBit(L, 4);
					break;
				case 0xED:
					L = setBit(L, 5);
					break;
				case 0xF5:
					L = setBit(L, 6);
					break;
				case 0xFD:
					L = setBit(L, 7);
					break;

				// SET b, (HL)
				// Sets bit b in memory at HL to 1
				case 0xC6:
					writeMem(getHL(), setBit(readMem(getHL()), 0));
					break;
				case 0xCE:
					writeMem(getHL(), setBit(readMem(getHL()), 1));
					break;
				case 0xD6:
					writeMem(getHL(), setBit(readMem(getHL()), 2));
					break;
				case 0xDE:
					writeMem(getHL(), setBit(readMem(getHL()), 3));
					break;
				case 0xE6:
					writeMem(getHL(), setBit(readMem(getHL()), 4));
					break;
				case 0xEE:
					writeMem(getHL(), setBit(readMem(getHL()), 5));
					break;
				case 0xF6:
					writeMem(getHL(), setBit(readMem(getHL()), 6));
					break;
				case 0xFE:
					writeMem(getHL(), setBit(readMem(getHL()), 7));
					break;

				// RES b, r
				// Set bit b in register r to 0

				// register A
				case 0x87:
					A = resetBit(A, 0);
					break;
				case 0x8F:
					A = resetBit(A, 1);
					break;
				case 0x97:
					A = resetBit(A, 2);
					break;
				case 0x9F:
					A = resetBit(A, 3);
					break;
				case 0xA7:
					A = resetBit(A, 4);
					break;
				case 0xAF:
					A = resetBit(A, 5);
					break;
				case 0xB7:
					A = resetBit(A, 6);
					break;
				case 0xBF:
					A = resetBit(A, 7);
					break;

				// register B
				case 0x80:
					B = resetBit(B, 0);
					break;
				case 0x88:
					B = resetBit(B, 1);
					break;
				case 0x90:
					B = resetBit(B, 2);
					break;
				case 0x98:
					B = resetBit(B, 3);
					break;
				case 0xA0:
					B = resetBit(B, 4);
					break;
				case 0xA8:
					B = resetBit(B, 5);
					break;
				case 0xB0:
					B = resetBit(B, 6);
					break;
				case 0xB8:
					B = resetBit(B, 7);
					break;

				// register C
				case 0x81:
					C = resetBit(C, 0);
					break;
				case 0x89:
					C = resetBit(C, 1);
					break;
				case 0x91:
					C = resetBit(C, 2);
					break;
				case 0x99:
					C = resetBit(C, 3);
					break;
				case 0xA1:
					C = resetBit(C, 4);
					break;
				case 0xA9:
					C = resetBit(C, 5);
					break;
				case 0xB1:
					C = resetBit(C, 6);
					break;
				case 0xB9:
					C = resetBit(C, 7);
					break;

				// register D
				case 0x82:
					D = resetBit(D, 0);
					break;
				case 0x8A:
					D = resetBit(D, 1);
					break;
				case 0x92:
					D = resetBit(D, 2);
					break;
				case 0x9A:
					D = resetBit(D, 3);
					break;
				case 0xA2:
					D = resetBit(D, 4);
					break;
				case 0xAA:
					D = resetBit(D, 5);
					break;
				case 0xB2:
					D = resetBit(D, 6);
					break;
				case 0xBA:
					D = resetBit(D, 7);
					break;

				// register E
				case 0x83:
					E = resetBit(E, 0);
					break;
				case 0x8B:
					E = resetBit(E, 1);
					break;
				case 0x93:
					E = resetBit(E, 2);
					break;
				case 0x9B:
					E = resetBit(E, 3);
					break;
				case 0xA3:
					E = resetBit(E, 4);
					break;
				case 0xAB:
					E = resetBit(E, 5);
					break;
				case 0xB3:
					E = resetBit(E, 6);
					break;
				case 0xBB:
					E = resetBit(E, 7);
					break;

				// register H
				case 0x84:
					H = resetBit(H, 0);
					break;
				case 0x8C:
					H = resetBit(H, 1);
					break;
				case 0x94:
					H = resetBit(H, 2);
					break;
				case 0x9C:
					H = resetBit(H, 3);
					break;
				case 0xA4:
					H = resetBit(H, 4);
					break;
				case 0xAC:
					H = resetBit(H, 5);
					break;
				case 0xB4:
					H = resetBit(H, 6);
					break;
				case 0xBC:
					H = resetBit(H, 7);
					break;

				// register L
				case 0x85:
					L = resetBit(L, 0);
					break;
				case 0x8D:
					L = resetBit(L, 1);
					break;
				case 0x95:
					L = resetBit(L, 2);
					break;
				case 0x9D:
					L = resetBit(L, 3);
					break;
				case 0xA5:
					L = resetBit(L, 4);
					break;
				case 0xAD:
					L = resetBit(L, 5);
					break;
				case 0xB5:
					L = resetBit(L, 6);
					break;
				case 0xBD:
					L = resetBit(L, 7);
					break;

				// RES b, (HL)
				// Reset bit b in memory at HL to 0
				case 0x86:
					writeMem(getHL(), resetBit(readMem(getHL()), 0));
					break;
				case 0x8E:
					writeMem(getHL(), resetBit(readMem(getHL()), 1));
					break;
				case 0x96:
					writeMem(getHL(), resetBit(readMem(getHL()), 2));
					break;
				case 0x9E:
					writeMem(getHL(), resetBit(readMem(getHL()), 3));
					break;
				case 0xA6:
					writeMem(getHL(), resetBit(readMem(getHL()), 4));
					break;
				case 0xAE:
					writeMem(getHL(), resetBit(readMem(getHL()), 5));
					break;
				case 0xB6:
					writeMem(getHL(), resetBit(readMem(getHL()), 6));
					break;
				case 0xBE:
					writeMem(getHL(), resetBit(readMem(getHL()), 7));
					break;
			}
			break;

		// JP nn
		// Jump to address specified by 16 bit immediate
		case 0xC3:
			PC = getImm16() - 1;
			break;

		// JP cc, nn
		// Jump to address specific if condition cc is met

		// Z == 0
		case 0xC2:
			if (!zero) {
				PC = getImm16() - 1;
			} else {
				PC += 2;
			}
			break;

		// Z == 1
		case 0xCA:
			if (zero) {
				PC = getImm16() - 1;
			} else {
				PC += 2;
			}
			break;

		// C == 0
		case 0xD2:
			if (!carry) {
				PC = getImm16() - 1;
			} else {
				PC += 2;
			}
			break;

		// C == 1
		case 0xDA:
			if (carry) {
				PC = getImm16() - 1;
			} else {
				PC += 2;
			}
			break;

		// JR e
		// Jump to PC + (signed) e
		case 0x18:
			PC += (char)getImm8();
			break;

		// JR cc, e
		// Jump to PC + (signed) e if condition cc is met

		// Z == 0
		case 0x20:
			if (!zero) {
				PC += (char)getImm8();
			} else {
				PC++;
			}
			break;

		// Z == 1
		case 0x28:
			if (zero) {
				PC += (char)getImm8();
			} else {
				PC++;
			}
			break;

		// C == 0
		case 0x30:
			if (!carry) {
				PC += (char)getImm8();
			} else {
				PC++;
			}
			break;

		// C == 1
		case 0x38:
			if (carry) {
				PC += (char)getImm8();
			} else {
				PC++;
			}
			break;

		// JP (HL)
		// Jump to address specified by register HL
		case 0xE9:
			PC = getHL() - 1;
			break;

		// CALL nn
		// Push current address onto stack and jump to address specified by 16 bit immediate
		case 0xCD:
			writeMem(SP--, (PC >> 8) & 0xFF);
			writeMem(SP--, PC & 0xFF);
			PC = getImm16() - 1;
			break;

		// CALL cc, nn
		// Call address at nn if condition cc is met

		// Z == 0
		case 0xC4:
			if (!zero) {
				writeMem(SP--, (PC >> 8) & 0xFF);
				writeMem(SP--, PC & 0xFF);
				PC = getImm16() - 1;
			}
			break;

		// Z == 1
		case 0xCC:
			if (zero) {
				writeMem(SP--, (PC >> 8) & 0xFF);
				writeMem(SP--, PC & 0xFF);
				PC = getImm16() - 1;
			}
			break;

		// C == 0
		case 0xD4:
			if (!carry) {
				writeMem(SP--, (PC >> 8) & 0xFF);
				writeMem(SP--, PC & 0xFF);
				PC = getImm16() - 1;
			}
			break;

		// C == 1
		case 0xDC:
			if (carry) {
				writeMem(SP--, (PC >> 8) & 0xFF);
				writeMem(SP--, PC & 0xFF);
				PC = getImm16() - 1;
			}
			break;

		// RET
		// Pop stack and return to popped address
		case 0xC9:
			PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			break;

		// RETI
		// Return from interrupt routine
		case 0xD9:
			PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			IME = true;
			break;

		// RET cc
		// Return from subroutine if condition cc is true

		// Z == 0
		case 0xC0:
			if (!zero) {
				PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			}
			PC += 2;
			break;

		// Z == 1
		case 0xC8:
			if (zero) {
				PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			}
			PC += 2;
			break;

		// C == 0
		case 0xD0:
			if (!carry) {
				PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			}
			PC += 2;
			break;

		// C == 1
		case 0xD8:
			if (carry) {
				PC = (readMem(++SP) | (readMem(++SP) << 8)) + 2;
			}
			PC += 2;
			break;

		// RST t
		// Load zero page memory address specified by t into PC
		case 0xC7:
			PC = 0xFFFF;
			break;
		case 0xCF:
			PC = 0x07;
			break;
		case 0xD7:
			PC = 0x0F;
			break;
		case 0xDF:
			PC = 0x17;
			break;
		case 0xE7:
			PC = 0x1F;
			break;
		case 0xEF:
			PC = 0x27;
			break;
		case 0xF7:
			PC = 0x2F;
			break;
		case 0xFF:
			PC = 0x37;
			break;

		// DAA
		// Decimal adjust accumulator
		case 0x27:
			decimalAdjustAcc();
			break;

		// CPL
		// Complement register A
		case 0x2F:
			A = ~A;
			break;

		// NOP
		// No operation
		case 0x00:
			break;

		// CCF
		// Complement carry flag
		case 0x3F:
			carry = !carry;
			break;

		// SCF
		// Set carry flag
		case 0x37:
			carry = true;
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

unsigned char GB_CPU::readMem(unsigned short addr) {
	return mem[addr];
}

void GB_CPU::writeMem(unsigned short addr, unsigned char value) {
	if (addr >= 0x8000) {
		rw.lock();
		mem[addr] = value;
		rw.unlock();
	}
	else {
		mem[addr] = value;
	}
}

// Add opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a + b (unsigned char)
unsigned char GB_CPU::add(unsigned char a, unsigned char b) {
	unsigned short result = a + b;
	zero = (result & 0xFF) == 0;
	halfCarry = (a & 0xF) + (b & 0xF) > 0xF;
	subtract = false;
	carry = result > 0xFF;
	return (unsigned char)result;
}

// Add carry opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a + b + carry (unsigned char)
unsigned char GB_CPU::addCarry(unsigned char a, unsigned char b) {
	unsigned short result = a + b + carry;
	zero = (result & 0xFF) == 0;
	halfCarry = (a & 0xF) + (b & 0xF) + carry > 0xF;
	subtract = false;
	carry = result > 0xFF;
	return (unsigned char)result;
}

// Add opertaion for 16-bit numbers
// Input: a (unsigned short), b (unsigned short)
// Output: a + b (unsigned short)
unsigned short GB_CPU::add16(unsigned short a, unsigned short b) {
	unsigned int result = a + b;
	halfCarry = (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
	subtract = false;
	carry = result > 0xFFFF;
	return (unsigned short)result;
}

// Subtract opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a - b (unsigned char)
unsigned char GB_CPU::sub(unsigned char a, unsigned char b) {
	short result = a - b;
	zero = result == 0;
	halfCarry = (a & 0xF) - (b & 0xF) < 0;
	subtract = true;
	carry = result < 0;
	return (unsigned char)result;
}

// Subtract carry opertaion for 8-bit numbers
// Input: a (unsigned char), b (unsigned char)
// Output: a - b - carry (unsigned char)
unsigned char GB_CPU::subCarry(unsigned char a, unsigned char b) {
	short result = a - b - (unsigned char)carry;
	zero = result == 0;
	halfCarry = (a & 0xF) - (b & 0xF) - carry < 0;
	subtract = true;
	carry = result < 0;
	return (unsigned char)result;
}

// ANDs two 8-bit numbers together
// Input: a (unsigned char), b (unsigned char)
// Output: a & b
unsigned char GB_CPU::bitAnd(unsigned char a, unsigned char b) {
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
unsigned char GB_CPU::bitOr(unsigned char a, unsigned char b) {
	unsigned char result = a | b;
	zero = result == 0;
	return result;
}

// XORs two 8-bit numbers together
// Input: a (unsigned char), b (unsigned char)
// Output: a ^ b
unsigned char GB_CPU::bitXor(unsigned char a, unsigned char b) {
	unsigned char result = a ^ b;
	zero = result == 0;
	return result;
}

// Increments value by one
// Input: value (unsigned char)
// Output: value + 1
unsigned char GB_CPU::inc(unsigned char value) {
	unsigned char result = value + 1;
	zero = result == 0;
	halfCarry = (value & 0xF) + 1 > 0xF;
	subtract = false;
	return result;
}

// Decrements value by one
// Input: value (unsigned char)
// Output: value - 1
unsigned char GB_CPU::dec(unsigned char value) {
	unsigned char result = value - 1;
	zero = result == 0;
	halfCarry = (value & 0xF) - 1 < 0;
	subtract = true;
	return result;
}

// Rotates value to the left
// Input: value (unsigned char)
// Output: rotated value
unsigned char GB_CPU::rotateLeft(unsigned char value) {
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
unsigned char GB_CPU::rotateLeftCarry(unsigned char value) {
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
unsigned char GB_CPU::rotateRight(unsigned char value) {
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
unsigned char GB_CPU::rotateRightCarry(unsigned char value) {
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
unsigned char GB_CPU::shiftLeft(unsigned char value) {
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
unsigned char GB_CPU::shiftRightArithmetic(unsigned char value) {
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
unsigned char GB_CPU::shiftRightLogical(unsigned char value) {
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
unsigned char GB_CPU::swap(unsigned char value) {
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
void GB_CPU::compBitToZero(unsigned char value, unsigned int bit) {
	zero = !(bool)((value >> bit) & 0x1);
	halfCarry = true;
	subtract = false;
}

// Sets specified bit to 1
// Input: value (unsigned char), bit (int)
// Output: value with specified bit set to 1 (unsigned char)
unsigned char GB_CPU::setBit(unsigned char value, unsigned int bit) {
	return value | (1 << bit);
}

// Sets specified bit to 0
// Input: value (unsigned char), bit (int)
// Output: value with specified bit set to 0 (unsigned char)
unsigned char GB_CPU::resetBit(unsigned char value, unsigned int bit) {
	return value & ~(1 << bit);
}

void GB_CPU::decimalAdjustAcc() {
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


// -------------------- REGISTER GETTERS / SETTERS --------------------
unsigned char GB_CPU::getF() {
	return 0x00 | zero << 7 | subtract << 6 | halfCarry << 5 | carry << 4;
}

unsigned short GB_CPU::getBC() {
	return B << 8 | C; 
}

unsigned short GB_CPU::getDE() {
	return D << 8 | E;
}

unsigned short GB_CPU::getHL() {
	return H << 8 | L;
}

unsigned char GB_CPU::getImm8() {
	PC++;
	return readMem(PC);
}

unsigned short GB_CPU::getImm16() {
	PC += 2;
	return readMem(PC) << 8 | readMem(PC - 1);
}

void GB_CPU::setF(unsigned char value) {
	zero = value >> 7 & 0x1;
	subtract = value >> 6 & 0x1;
	halfCarry = value >> 5 & 0x1;
	carry = value >> 4 & 0x1;
}

void GB_CPU::setBC(unsigned short value) {
	B = value >> 8;
	C = (unsigned char)value;
}

void GB_CPU::setDE(unsigned short value) {
	D = value >> 8; 
	E = (unsigned char)value;
}

void GB_CPU::setHL(unsigned short value) {
	H = value >> 8;
	L = (unsigned char)value;
}
