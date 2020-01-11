#include "pch.h"
#include "../Dot Matrix/GB_CPU.h"

GB_CPU gb;

TEST(TestAddFunction, TestCPU) {
	
	// standard case
	char a = 0x34;
	char b = 0x43;
	char result1 = gb.add(a, b);

	ASSERT_EQ(result1, 0x77);
	ASSERT_FALSE(gb.getCarry());
	ASSERT_TRUE(gb.getHalfCarry());
	ASSERT_FALSE(gb.getSubtract());
	ASSERT_FALSE(gb.getZero());
}