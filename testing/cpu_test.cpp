#include "../cpu.h"
#include "gtest/gtest.h"

static CPU gb;

TEST(TestArithmetic, TestAdd) {

	// 0x3A + 0xC6
	ASSERT_EQ(gb.add(0x3A, 0xC6, NO_CARRY), 0x0);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0x3C + 0xFF
	ASSERT_EQ(gb.add(0x3C, 0xFF, NO_CARRY), 0x3B);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0x3C + 0x12
	ASSERT_EQ(gb.add(0x3C, 0x12, NO_CARRY), 0x4E);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);
}

TEST(TestArithmetic, TestAdd16) {

	// 0x8A23 + 0x0605
	ASSERT_EQ(gb.add16(0x8A23, 0x0605), 0x9028);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x8A23 + 0x1446
	ASSERT_EQ(gb.add16(0x8A23, 0x8A23), 0x1446);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);
}

TEST(TestArithmetic, TestAddCarry) {

	// 0xE1 + 0x0F + 1
	gb.carry = true;
	ASSERT_EQ(gb.add(0xE1, 0x0F, WITH_CARRY), 0xF1);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0xE1 + 0x3B + 1
	gb.carry = true;
	ASSERT_EQ(gb.add(0xE1, 0x3B, WITH_CARRY), 0x1D);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);

	// 0xE1 + 0x1E;
	gb.carry = true;
	ASSERT_EQ(gb.add(0xE1, 0x1E, WITH_CARRY), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_TRUE(gb.carry);
}

TEST(TestArithmetic, TestDecimalAdjust) {

	// 0x45 + 0x38
	gb.A = gb.add(0x45, 0x38, NO_CARRY);
	gb.decimalAdjustAcc();
	ASSERT_EQ(gb.A, 0x83);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.zero);

	// 0x83 - 0x38
	gb.A = gb.sub(gb.A, 0x38, NO_CARRY);
	gb.decimalAdjustAcc();
	ASSERT_EQ(gb.A, 0x45);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.zero);
}

TEST(TestArithmetic, TestDecrement) {

	// 0x01 - 1
	ASSERT_EQ(gb.dec(0x01), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);

	// 0x00 - 1
	ASSERT_EQ(gb.dec(0x00), 0xFF);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
}

TEST(TestArithmetic, TestIncrement) {

	// 0xFF + 1
	ASSERT_EQ(gb.inc(0xFF), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x50 + 1
	ASSERT_EQ(gb.inc(0x50), 0x51);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestArithmetic, TestSub) {

	// 0x3E - 0x3E
	ASSERT_EQ(gb.sub(0x3E, 0x3E, NO_CARRY), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x3E - 0x0F
	ASSERT_EQ(gb.sub(0x3E, 0x0F, NO_CARRY), 0x2F);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x3E - 0x40
	ASSERT_EQ(gb.sub(0x3E, 0x40, NO_CARRY), 0xFE);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_TRUE(gb.carry);
}

TEST(TestArithmetic, TestSubCarry) {

	// 0x3B - 0x2A - 1
	gb.carry = true;
	ASSERT_EQ(gb.sub(0x3B, 0x2A, WITH_CARRY), 0x10);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x3B - 0x3A - 1
	gb.carry = true;
	ASSERT_EQ(gb.sub(0x3B, 0x3A, WITH_CARRY), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x3B - 0x4F - 1
	gb.carry = true;
	ASSERT_EQ(gb.sub(0x3B, 0x4F, WITH_CARRY), 0xEB);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_TRUE(gb.subtract);
	ASSERT_TRUE(gb.carry);
}

TEST(TestDecode, TestStack) {
    gb.SP = 0xFFFE;
    gb.setRegPair(BC, 0x1234);
    gb.setRegPair(DE, 0x5678);

	gb.decode(0xC5); // push BC to stack
	gb.decode(0xD5); // push DE to stack

	ASSERT_EQ(gb.SP, 0xFFFA);

	gb.decode(0xC1); // pop stack and store in BC
	gb.decode(0xD1); // pop stack and store in DE

	ASSERT_EQ(gb.SP, 0xFFFE);

	ASSERT_EQ(gb.getRegPair(BC), 0x5678);
	ASSERT_EQ(gb.getRegPair(DE), 0x1234);
}

TEST(TestLogical, TestBitAnd) {

	// 0x5A & 0x3F
	ASSERT_EQ(gb.bitAnd(0x5A, 0x3F), 0x1A);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x5A & 0x38
	ASSERT_EQ(gb.bitAnd(0x5A, 0x38), 0x18);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// 0x5A & 0x00
	ASSERT_EQ(gb.bitAnd(0x5A, 0x00), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);
}

TEST(TestLogical, TestBitOr) {

	// 0x5A | 0x5A
	ASSERT_EQ(gb.bitOr(0x5A, 0x5A), 0x5A);
	ASSERT_FALSE(gb.zero);

	// 0x5A | 0x5B
	ASSERT_EQ(gb.bitOr(0x5A, 0x5B), 0x5B);
	ASSERT_FALSE(gb.zero);

	// 0x5A | 0x0F
	ASSERT_EQ(gb.bitOr(0x5A, 0x0F), 0x5F);
	ASSERT_FALSE(gb.zero);

	// 0x00 | 0x00
	ASSERT_EQ(gb.bitOr(0x00, 0x00), 0x00);
	ASSERT_TRUE(gb.zero);
}

TEST(TestLogical, TestBitXor) {

	// 0xFF ^ 0xFF
	ASSERT_EQ(gb.bitXor(0xFF, 0xFF), 0x00);
	ASSERT_TRUE(gb.zero);

	// 0xFF ^ 0x0F
	ASSERT_EQ(gb.bitXor(0xFF, 0x0F), 0xF0);
	ASSERT_FALSE(gb.zero);

	// 0xFF ^ 0x8A
	ASSERT_EQ(gb.bitXor(0xFF, 0x8A), 0x75);
	ASSERT_FALSE(gb.zero);
}

TEST(TestLogical, TestCompBitToZero) {
	gb.compBitToZero(0xFE, 0);
	ASSERT_TRUE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	gb.compBitToZero(0xFE, 1);
	ASSERT_FALSE(gb.zero);
	ASSERT_TRUE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestResetBit) {
	ASSERT_EQ(gb.resetBit(0x80, 7), 0x00);
	ASSERT_EQ(gb.resetBit(0x3B, 1), 0x39);
}

TEST(TestLogical, TestRotateLeft) {

	// 0x85 rotate left 1
	ASSERT_EQ(gb.rotateLeft(0x85), 0x0B);
	ASSERT_TRUE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x70 rotate left 1
	ASSERT_EQ(gb.rotateLeft(0x40), 0x80);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestRotateLeftCarry) {

	// 0x95 rotate left 1 through carry = 1
	gb.carry = 1;
	ASSERT_EQ(gb.rotateLeftCarry(0x95), 0x2B);
	ASSERT_TRUE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x95 rotate left 1 through carry = 0
	gb.carry = false;
	ASSERT_EQ(gb.rotateLeftCarry(0x95), 0x2A);
	ASSERT_TRUE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestRotateRight) {

	// 0x3B rotate right 1
	ASSERT_EQ(gb.rotateRight(0x3B), 0x9D);
	ASSERT_TRUE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x80 rotate right 1
	ASSERT_EQ(gb.rotateRight(0x80), 0x40);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestRotateRightCarry) {

	// 0x81 rotate right 1 through carry = 0
	gb.carry = false;
	ASSERT_EQ(gb.rotateRightCarry(0x81), 0x40);
	ASSERT_TRUE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x80 rotate right 1 through carry = 1
	gb.carry = true;
	ASSERT_EQ(gb.rotateRightCarry(0x80), 0xC0);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestSetBit) {
	ASSERT_EQ(gb.setBit(0x80, 3), 0x88);
	ASSERT_EQ(gb.setBit(0x3B, 7), 0xBB);
}

TEST(TestLogical, TestShiftLeft) {

	// 0x80 << 1
	ASSERT_EQ(gb.shiftLeft(0x80), 0x00);
	ASSERT_TRUE(gb.carry);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x7F << 1
	ASSERT_EQ(gb.shiftLeft(0x7F), 0xFE);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestShiftRightArithmetic) {

	// 0x8A >> 1
	ASSERT_EQ(gb.shiftRightArithmetic(0x8A), 0xC5);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x01 >> 1
	ASSERT_EQ(gb.shiftRightArithmetic(0x01), 0x00);
	ASSERT_TRUE(gb.carry);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestShiftRightLogical) {

	// 0x01 >> 1
	ASSERT_EQ(gb.shiftRightLogical(0x01), 0x00);
	ASSERT_TRUE(gb.carry);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);

	// 0x01 >> 1
	ASSERT_EQ(gb.shiftRightLogical(0xFE), 0x7F);
	ASSERT_FALSE(gb.carry);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
}

TEST(TestLogical, TestSwap) {

	// swap 0x00
	ASSERT_EQ(gb.swap(0x00), 0x00);
	ASSERT_TRUE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);

	// swap 0xF4
	ASSERT_EQ(gb.swap(0xF4), 0x4F);
	ASSERT_FALSE(gb.zero);
	ASSERT_FALSE(gb.halfCarry);
	ASSERT_FALSE(gb.subtract);
	ASSERT_FALSE(gb.carry);
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
	gb.setRegPair(DE, 0x9876);

	ASSERT_EQ(gb.getRegPair(BC), 0x1234);
	ASSERT_EQ(gb.D, 0x98);
	ASSERT_EQ(gb.E, 0x76);
}
