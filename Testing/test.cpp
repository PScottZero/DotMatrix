#include "pch.h"
#include "../Dot Matrix/GB_CPU.h"
#include "../Dot Matrix/GB_CPU.cpp"

constexpr auto REG_COUNT = 7;
GB_CPU gb;

TEST(TestDecode, TestLD) {
	unsigned char values[REG_COUNT] = { 0, 1, 2, 3, 4, 5, 6 };
	unsigned char* regs[REG_COUNT] = { &gb.A, &gb.B, &gb.C, &gb.D, &gb.E, &gb.H, &gb.L };
	for (int index = 0; index < REG_COUNT; index++) {
		*regs[index] = values[index];
	}

	// LD A, reg
	unsigned char opcodesA[REG_COUNT] = { 0x7F, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesA[index]);
		ASSERT_EQ(gb.A, values[index]);
		gb.A = values[0];
	}

	// LD B, reg
	unsigned char opcodesB[REG_COUNT] = { 0x47, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45 };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesB[index]);
		ASSERT_EQ(gb.B, values[index]);
		gb.B = values[1];
	}

	// LD C, reg
	unsigned char opcodesC[REG_COUNT] = { 0x4F, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesC[index]);
		ASSERT_EQ(gb.C, values[index]);
		gb.C = values[2];
	}

	// LD D, reg
	unsigned char opcodesD[REG_COUNT] = { 0x57, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55 };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesD[index]);
		ASSERT_EQ(gb.D, values[index]);
		gb.D = values[3];
	}

	// LD E, reg
	unsigned char opcodesE[REG_COUNT] = { 0x5F, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesE[index]);
		ASSERT_EQ(gb.E, values[index]);
		gb.E = values[4];
	}

	// LD H, reg
	unsigned char opcodesH[REG_COUNT] = { 0x67, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65 };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesH[index]);
		ASSERT_EQ(gb.H, values[index]);
		gb.H = values[5];
	}

	// LD L, reg
	unsigned char opcodesL[REG_COUNT] = { 0x6F, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D };
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesL[index]);
		ASSERT_EQ(gb.L, values[index]);
		gb.L = values[6];
	}

	// LD reg, n
	unsigned char opcodesImm[REG_COUNT] = { 0x3E, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E };
	gb.PC = 0;
	gb.mem[gb.PC + 1] = 0x12;
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesImm[index]);
		ASSERT_EQ(*regs[index], 0x12);
		ASSERT_EQ(gb.PC, 2);
		gb.PC -= 2;
	}

	// LD reg, (HL)
	unsigned char opcodesLoadHL[REG_COUNT] = { 0x7E, 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E };
	gb.setHL(0x1234);
	gb.mem[gb.getHL()] = 0x34;
	unsigned char yes = gb.mem[gb.getHL()];
	for (int index = 0; index < REG_COUNT; index++) {
		gb.decode(opcodesLoadHL[index]);
		ASSERT_EQ(*regs[index], 0x34);
		gb.setHL(0x1234);
	}
}

TEST(TestDecode, TestStack) {
	gb.setBC(0x1234);
	gb.setDE(0x5678);

	gb.decode(0xC5); // push BC to stack
	gb.decode(0xD5); // push DE to stack

	ASSERT_EQ(gb.SP, 0xFFFA);

	gb.decode(0xC1); // pop stack and store in BC
	gb.decode(0xD1); // pop stack and store in DE

	ASSERT_EQ(gb.SP, 0xFFFE);

	ASSERT_EQ(gb.getBC(), 0x5678);
	ASSERT_EQ(gb.getDE(), 0x1234);
}

TEST(TestHelper, TestAdd) {

	// 0x3A + 0xC6
	unsigned char result = gb.add(0x3A, 0xC6);
	
	ASSERT_EQ(result, 0x0);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0x3C + 0xFF
	result = gb.add(0x3C, 0xFF);
	
	ASSERT_EQ(result, 0x3B);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0x3C + 0x12
	result = gb.add(0x3C, 0x12);
	
	ASSERT_EQ(result, 0x4E);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);
}

TEST(TestHelper, TestAddCarry) {
	
	// 0xE1 + 0x0F + 1
	gb.carry = 1;
	unsigned char result = gb.addCarry(0xE1, 0x0F);

	ASSERT_EQ(result, 0xF1);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0xE1 + 0x3B + 1
	gb.carry = 1;
	result = gb.addCarry(0xE1, 0x3B);

	ASSERT_EQ(result, 0x1D);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0xE1 + 0x1E;
	gb.carry = 1;
	result = gb.addCarry(0xE1, 0x1E);

	ASSERT_EQ(result, 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);
}

TEST(TestReg, TestRegF) {
	gb.zero = true;
	gb.halfCarry = false;
	gb.subtract = true;
	gb.carry = false;

	ASSERT_EQ(gb.getF(), 0xC0);

	gb.zero = false;
	gb.halfCarry = true;
	gb.subtract = false;
	gb.carry = true;
	gb.setF(0xC0);

	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_FALSE(gb.carry);
}

TEST(TestReg, TestReg16) {
	gb.B = 0x12;
	gb.C = 0x34;
	gb.setDE(0x9876);

	ASSERT_EQ(gb.getBC(), 0x1234);
	ASSERT_EQ(gb.D, 0x98);
	ASSERT_EQ(gb.E, 0x76);
}
