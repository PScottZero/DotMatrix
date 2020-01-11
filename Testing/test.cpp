#include "pch.h"
#include "../Dot Matrix/GB_CPU.h"
#include "../Dot Matrix/GB_CPU.cpp"

GB_CPU gb;


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
