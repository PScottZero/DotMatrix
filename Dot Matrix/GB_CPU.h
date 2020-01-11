#pragma once
#include <iostream>
#include <fstream>
#include <array>

using namespace std;

class GB_CPU
{
public:
	short AF, BC, DE, HL, PC, SP;
	char* mem;
	char* cart;
	bool IME;

	GB_CPU();

	// emulator functions
	void decode();
	void loadBootstrap();
	void loadCartridge(string dir);
	void run();
	
	// opcode functions
	char add(char a, char b);
	short add16(short a, short b);
	char bitAnd(char a, char b);
	char bitOr(char a, char b);
	char bitXor(char a, char b);
	void compBitToZero(char value, int bit);
	char dec(char value);
	char inc(char value);
	char resetBit(char value, int bit);
	char rotateLeft(char value);
	char rotateLeftCarry(char value);
	char rotateRight(char value);
	char rotateRightCarry(char value);
	char shiftLeft(char value);
	char shiftRightArithmetic(char value);
	char shiftRightLogical(char value);
	char sub(char a, char b);
	char swap(char value);
	char setBit(char value, int bit);
	
	// 8-bit register functions
	char getA();
	char getB();
	char getC();
	char getD();
	char getE();
	char getF();
	char getH();
	char getL();
	void setA(char value);
	void setB(char value);
	void setC(char value);
	void setD(char value);
	void setE(char value);
	void setF(char value);
	void setH(char value);
	void setL(char value);

	// 16-bit register functions
	short getAF();
	short getBC();
	short getDE();
	short getHL();
	void setAF(short value);
	void setBC(short value);
	void setDE(short value);
	void setHL(short value);

	// flag functions
	char getCarry();
	char getHalfCarry();
	char getSubtract();
	char getZero();
	void setCarry(char value);
	void setHalfCarry(char value);
	void setSubtract(char value);
	void setZero(char value);
};