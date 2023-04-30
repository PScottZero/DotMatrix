#include "cpu.h"

#include "json.hpp"

CPU::CPU(Memory &mem, float &speedMult, bool &stop, bool &threadRunning,
         bool &bootstrapMode)
    : QThread(),
      PC(),
      SP(),
      BC(),
      DE(),
      HL(),
      A(),
      B(*((uint8 *)&BC + 1)),
      C(*((uint8 *)&BC)),
      D(*((uint8 *)&DE + 1)),
      E(*((uint8 *)&DE)),
      H(*((uint8 *)&HL + 1)),
      L(*((uint8 *)&HL)),
      zero(true),
      subtract(false),
      halfCarry(true),
      carry(true),
      IME(false),
      halt(false),
      stop(stop),
      regmap8{&B, &C, &D, &E, &H, &L, nullptr, &A},
      regmap16{&BC, &DE, &HL, &SP},
      mem(mem),
      clock(),
      speedMult(speedMult),
      threadRunning(threadRunning),
      bootstrapMode(bootstrapMode),
      shouldSetIME(false) {}

CPU::~CPU() {
  threadRunning = false;
  wait();
}

void CPU::run() {
  std::fstream fs("/Users/paulscott/git/DotMatrix/debug/cpu_log.txt",
                  std::ios::out);
  char logLine[256];

  clock.reset();
  bool delaySetIME = true;
  while (threadRunning) {
    if (!stop) {
      // get current system time
      uint8 cycles = 0;

      // check for interrupts
      handleInterrupts(cycles);

      // run instruction at current PC
      if (!halt) {
        log(fs);
        runInstr(mem.imm8(PC, cycles), cycles);
      } else {
        cycles = 1;
      }

      // delay setting IME after an EI
      // instruction by one machine cycle
      if (shouldSetIME) {
        if (!delaySetIME) {
          IME = true;
          shouldSetIME = false;
          delaySetIME = true;
        } else {
          delaySetIME = false;
        }
      }

      // wait until the time corresponding to the number
      // of cycles has passed
      int cycleTimeNs = NS_PER_SEC / (DMG_CLOCK_SPEED * speedMult);
      clock.wait(cycles * cycleTimeNs);
    } else {
      clock.reset();
    }
  }
  fs.close();
}

// **************************************************
// **************************************************
// Instruction Decoding Functions
// **************************************************
// **************************************************

// run the instruction specified by the given
// 8-bit opcode
void CPU::runInstr(uint8 opcode, uint8 &cycles) {
  // break 8-bit opcode into parts
  uint8 upperTwoBits = (opcode >> 6) & 0b11;
  uint8 regDest = (opcode >> 3) & 0b111;
  uint8 regSrc = opcode & 0b111;
  uint8 regPair = (regDest >> 1) & 0b11;
  uint8 jumpCond = regDest & 0b11;
  uint8 loNibble = opcode & 0xF;

  bool prevCarry = carry;
  bool prevZero = zero;

  regmap8[MEM_HL] = mem.getBytePtr(HL);

  // instruction comment format:
  // INSTR PARAM1, PARAM2
  // #cycles carry/half-carry/subtract/zero
  // description of instruction

  // the following instructions are
  // represented by only a single
  // 8-bit opcode value
  switch (opcode) {
    // ==================================================
    // 8-Bit Transfer and I/O Instructions
    // ==================================================

    // LD (HL), n
    // 3 ----
    // load 8-bit immediate n into memory at HL
    case 0x36:
      mem.write(HL, mem.imm8(PC, cycles), cycles);
      break;

    // LD A, (BC)
    // 2 ----
    // load memory at BC into accumulator
    case 0x0A:
      A = mem.read(BC, cycles);
      break;

    // LD A, (DE)
    // 2 ----
    // load memory at DE into accumulator
    case 0x1A:
      A = mem.read(DE, cycles);
      break;

    // LD A, (C)
    // 2 ----
    // load memory at FF00 + C into accumulator
    case 0xF2:
      A = mem.read(ZERO_PAGE_ADDR + C, cycles);
      break;

    // LD (C), A
    // 2 ----
    // load accumulator into memory at FF00 + C
    case 0xE2:
      mem.write(ZERO_PAGE_ADDR + C, A, cycles);
      break;

    // LD A, (n)
    // 3 ----
    // load memory at FF00 + 8-bit immediate n
    // into accumulator
    case 0xF0:
      A = mem.read(ZERO_PAGE_ADDR + mem.imm8(PC, cycles), cycles);
      break;

    // LD (n), A
    // 3 ----
    // load accumulator into memory at
    // FF00 + 8-bit immediate n
    case 0xE0:
      mem.write(ZERO_PAGE_ADDR + mem.imm8(PC, cycles), A, cycles);
      break;

    // LD A, (nn)
    // 4 ----
    // load memory at 16-bit immediate nn
    // into accumulator
    case 0xFA:
      A = mem.read(mem.imm16(PC, cycles), cycles);
      break;

    // LD (nn), A
    // 4 ----
    // load accumulator into memory at
    // 16-bit immediate nn
    case 0xEA:
      mem.write(mem.imm16(PC, cycles), A, cycles);
      break;

    // LD A, (HLI)
    // 2 ----
    // load memory at HL into accumulator
    // then increment HL
    case 0x2A:
      A = mem.read(HL++, cycles);
      break;

    // LD A, (HLD)
    // 2 ----
    // load memory at HL into accumulator
    // then decrement HL
    case 0x3A:
      A = mem.read(HL--, cycles);
      break;

    // LD (BC), A
    // 2 ----
    // load accumulator into memory at BC
    case 0x02:
      mem.write(BC, A, cycles);
      break;

    // LD (DE), A
    // 2 ----
    // load accumulator into memory at DE
    case 0x12:
      mem.write(DE, A, cycles);
      break;

    // LD (HLI), A
    // 2 ----
    // load accumulator into memory at HL
    // then increment HL
    case 0x22:
      mem.write(HL++, A, cycles);
      break;

    // LD (HLD), A
    // 2 ----
    // load accumulator into memory at HL
    // then decrement HL
    case 0x32:
      mem.write(HL--, A, cycles);
      break;

    // ==================================================
    // 16-Bit Transfer Instructions
    // ==================================================

    // LD SP, HL
    // 2 ----
    // load register HL into the stack pointer
    case 0xF9:
      SP = HL;
      ++cycles;
      break;

    // LDHL SP, e
    // 3 CH00
    // load the stack pointer + 8-bit signed
    // immediate e into register HL
    case 0xF8:
      HL = addSP(mem.imm8(PC, cycles));
      ++cycles;
      break;

    // LD (nn), SP
    // 5 ----
    // load the stack pointer into memory at
    // 16-bit immediate nn
    case 0x08:
      mem.write(mem.imm16(PC, cycles), SP, cycles);
      break;

    // ==================================================
    // 8-Bit Arithmetic and Logical Instructions
    // ==================================================

    // ADD A, n
    // 2 CH0Z
    // add 8-bit immediate n to accumulator and
    // store result in accumulator
    case 0xC6:
      A = add(A, mem.imm8(PC, cycles));
      break;

    // ADC A, n
    // 2 CH0Z
    // add 8-bit immediate n and carry to
    // accumulator and store result
    // in accumulator
    case 0xCE:
      A = add(A, mem.imm8(PC, cycles), carry);
      break;

    // SUB A, n
    // 2 CH1Z
    // subtract 8-bit immediate n to accumulator
    // and store result in accumulator
    case 0xD6:
      A = sub(A, mem.imm8(PC, cycles));
      break;

    // SBC A, n
    // 2 CH1Z
    // subtract 8-bit immediate n and carry from
    // accumulator and store result
    // in accumulator
    case 0xDE:
      A = sub(A, mem.imm8(PC, cycles), carry);
      break;

    // AND A, n
    // 2 010Z
    // and 8-bit immediate n with accumulator
    // and store result in accumulator
    case 0xE6:
      _and(mem.imm8(PC, cycles));
      break;

    // XOR A, n
    // 2 000Z
    // xor 8-bit immediate n with accumulator
    // and store result in accumulator
    case 0xEE:
      _xor(mem.imm8(PC, cycles));
      break;

    // OR A, n
    // 2 000Z
    // or 8-bit immediate n with accumulator
    // and store result in accumulator
    case 0xF6:
      _or(mem.imm8(PC, cycles));
      break;

    // CP A, n
    // 2 CH1Z
    // compare 8-bit immediate n with accumulator
    case 0xFE:
      sub(A, mem.imm8(PC, cycles));
      break;

    // ==================================================
    // 16-Bit Arithmetic Instructions
    // ==================================================

    // ADD SP, e
    // 4 CH00
    // add 8-bit signed immediate e to stack pointer
    // and store result in stack pointer
    case 0xE8:
      SP = addSP(mem.imm8(PC, cycles));
      cycles += 2;
      break;

    // ==================================================
    // Rotate Shift Instructions
    // ==================================================

    // RLCA
    // 1 C000
    // rotate register A to the left
    case 0x07:
      A = rotateLeft(A);
      zero = false;
      break;

    // RLA
    // 1 C000
    // rotate register A to the left through
    // the carry flag
    case 0x17:
      A = rotateLeft(A, true);
      zero = false;
      break;

    // RRCA
    // 1 C000
    // rotate register A to the right
    case 0x0F:
      A = rotateRight(A);
      zero = false;
      break;

    // RRA
    // 1 C000
    // rotate register A to the right through
    // the carry flag
    case 0x1F:
      A = rotateRight(A, true);
      zero = false;
      break;

    // ==================================================
    // CB Prefixed Instructions
    // ==================================================

    // run instruction with CB prefix
    case 0xCB:
      runInstrCB(mem.imm8(PC, cycles), cycles);
      break;

    // ==================================================
    // Jump, Call, and Return Instructions
    // ==================================================

    // JP nn
    // 4 ----
    // jump to 16-bit immediate address nn
    case 0xC3:
      PC = mem.imm16(PC, cycles);
      ++cycles;
      break;

    // JR e
    // 3 ----
    // jump to address PC + 8-bit signed
    // immediate e
    case 0x18:
      PC += (int8)mem.imm8(PC, cycles);
      ++cycles;
      break;

    // JP (HL)
    // 1 ----
    // jump to address HL
    case 0xE9:
      PC = HL;
      break;

    // CALL nn
    // 6 ----
    // push PC onto stack and jump to
    // 16-bit immediate address
    case 0xCD:
      push(PC + 2, cycles);
      PC = mem.imm16(PC, cycles);
      break;

    // RET
    // 4 ----
    // return from subroutine
    case 0xC9:
      PC = pop(cycles);
      ++cycles;
      break;

    // RETI
    // 4 ----
    // return from interrupt
    case 0xD9:
      PC = pop(cycles);
      IME = true;
      ++cycles;
      break;

    // ==================================================
    // General-Purpose Arithmetic Operations and
    // CPU Control Instructions
    // ==================================================

    // DAA
    // 1 C0-Z
    // decimal adjust the accumulator after
    // adding or subtracting two binary
    // encoded decimals
    case 0x27:
      decimalAdjAcc();
      break;

    // CPL
    // 1 -11-
    // take ones complement of accumulator
    case 0x2F:
      A = ~A;
      halfCarry = true;
      subtract = true;
      break;

    // NOP
    // 1 ----
    // do nothing
    case 0x00:
      break;

    // CCF
    // 1 C00-
    // flip carry flag
    case 0x3F:
      carry = !carry;
      halfCarry = false;
      subtract = false;
      break;

    // SCF
    // 1 100-
    // set carry flag
    case 0x37:
      carry = true;
      halfCarry = false;
      subtract = false;
      break;

    // DI
    // 1 ----
    // reset interrupt master enable flag
    case 0xF3:
      IME = false;
      break;

    // EI
    // 1 ----
    // set interrupt master enable flag
    case 0xFB:
      shouldSetIME = true;
      break;

    // HALT
    // 1 ----
    // halts the cpu and system clock
    case 0x76:
      halt = true;
      if (!IME && Interrupts::pending()) halt = false;
      break;

    // STOP
    // 1 ----
    // halts the cpu, system clock, oscillator,
    // and lcd controller
    case 0x10:
      if (mem.imm8(PC, cycles) == 0) {
        stop = true;
      }
      --cycles;
      break;

    // illegal opcodes
    case 0xD3:
    case 0xDB:
    case 0xDD:
    case 0xE3:
    case 0xE4:
    case 0xEB:
    case 0xEC:
    case 0xED:
    case 0xF4:
    case 0xFC:
    case 0xFD:
      printf("Illegal opcode %02x\n", opcode);
      break;

    // the following instructions are
    // represented by a range of
    // 8-bit opcode values
    default:
      switch (upperTwoBits) {
        case 0b00:
          switch (loNibble) {
            // LD dd, nn
            // 3 ----
            // load 16-bit immediate nn into register pair dd
            case 0x1:
              *regmap16[regPair] = mem.imm16(PC, cycles);
              break;

            // INC ss
            // 2 ----
            // increment register pair ss
            case 0x3:
              ++*regmap16[regPair];
              ++cycles;
              break;

            // ADD HL, ss
            // 2 CH0-
            // add register pair ss to HL and store
            // result in HL
            case 0x9:
              HL = add(HL, *regmap16[regPair]);
              zero = prevZero;
              ++cycles;
              break;

            // DEC ss
            // 2 ----
            // decrement register pair ss
            case 0xB:
              --*regmap16[regPair];
              break;

            default:
              switch (regSrc) {
                // JR cc, e
                // 3/2 ----
                // jump to address PC + 8-bit signed
                // immediate e if jump condition cc is met
                case 0b000:
                  if (jumpCondMet(jumpCond)) {
                    PC += (int8)mem.imm8(PC, cycles);
                    ++cycles;
                  } else {
                    ++PC;
                    cycles += 1;
                  }
                  break;

                // INC r / INC (HL)
                // 1/3 -H0Z
                // increment register r (or memory at HL)
                case 0b100:
                  *regmap8[regDest] = add(*regmap8[regDest], 1);
                  carry = prevCarry;
                  if (regDest == MEM_HL) cycles += 2;
                  break;

                // DEC r / DEC (HL)
                // 1/3 -H1Z
                // decrement register r (or memory at HL)
                case 0b101:
                  *regmap8[regDest] = sub(*regmap8[regDest], 1);
                  carry = prevCarry;
                  if (regDest == MEM_HL) cycles += 2;
                  break;

                // LD r, n / LD (HL), n
                // 2/3 ----
                // load 8-bit immediate n into register r
                // (or memory at HL)
                case 0b110:
                  *regmap8[regDest] = mem.imm8(PC, cycles);
                  if (regDest == MEM_HL) ++cycles;
                  break;
              }
              break;
          }
          break;

        case 0b01:
          // LD r, r' / LD r, (HL) / LD (HL), r
          // 1/2/2 ----
          // load value of register r' (or memory at HL)
          // into register r (or memory at HL)
          *regmap8[regDest] = *regmap8[regSrc];
          if (regSrc == MEM_HL || regDest == MEM_HL) ++cycles;
          break;

        case 0b10:
          switch (regDest) {
            // ADD A, r / ADD A, (HL)
            // 1/2 CH0Z
            // add register r (or memory at HL) to
            // accumulator and store result
            // in accumulator
            case 0b000:
              A = add(A, *regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // ADC A, r / ADC A, (HL)
            // 1/2 CH0Z
            // add register r (or memory at HL) and
            // carry to accumulator and store result
            // in accumulator
            case 0b001:
              A = add(A, *regmap8[regSrc], carry);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // SUB A, r / SUB A, (HL)
            // 1/2 CH1Z
            // subtract register r (or memory at HL)
            // from accumulator and store result
            // in accumulator
            case 0b010:
              A = sub(A, *regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // SBC A, r / SBC A, (HL)
            // 1/2 CH1Z
            // subtract register r (or memory at HL) and
            // carry from accumulator and store result
            // in accumulator
            case 0b011:
              A = sub(A, *regmap8[regSrc], carry);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // AND A, r / AND A, (HL)
            // 1/2 010Z
            // and register r (or memory at HL) with
            // accumulator and store result in accumulator
            case 0b100:
              _and(*regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // XOR A, r / XOR A, (HL)
            // 1/2 000Z
            // xor register r (or memory at HL) with
            // accumulator and store result in accumulator
            case 0b101:
              _xor(*regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // OR A, r / OR A, (HL)
            // 1/2 000Z
            // or register r (or memory at HL) with
            // accumulator and store result in accumulator
            case 0b110:
              _or(*regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;

            // CP A, r / CP A, (HL)
            // 1/2 CH1Z
            // compare register r (or memory at HL)
            // with accumulator
            case 0b111:
              sub(A, *regmap8[regSrc]);
              if (regSrc == MEM_HL) ++cycles;
              break;
          }
          break;

        case 0b11:
          switch (loNibble) {
            // POP qq
            // 3 ----
            // pop the stack and store popped
            // content in register pair qq
            case 0x1:
              if (regPair != 0b11) {
                *regmap16[regPair] = pop(cycles);
              } else {
                setAF(pop(cycles));
              }
              break;

            // PUSH qq
            // 4 ----
            // push register pair qq onto stack
            case 0x5:
              push(regPair != 0b11 ? *regmap16[regPair] : getAF(), cycles);
              break;

            default:
              switch (regSrc) {
                // RET cc
                // 5/2 ----
                // return from subroutine if jump condition
                // cc is met
                case 0b000:
                  if (jumpCondMet(jumpCond)) {
                    PC = pop(cycles);
                    cycles += 2;
                  } else {
                    ++cycles;
                  }
                  break;

                // JP cc, nn
                // 4/3 ----
                // jump to address nn if jump condition
                // cc is met
                case 0b010:
                  if (jumpCondMet(jumpCond)) {
                    PC = mem.imm16(PC, cycles);
                    ++cycles;
                  } else {
                    PC += 2;
                    cycles += 2;
                  }
                  break;

                // CALL cc, nn
                // 6/3 ----
                // push PC onto stack and jump to 16-bit
                // immediate address nn if jump condition
                // cc is met
                case 0b100:
                  if (jumpCondMet(jumpCond)) {
                    push(PC + 2, cycles);
                    PC = mem.imm16(PC, cycles);
                  } else {
                    PC += 2;
                    cycles += 2;
                  }
                  break;

                // RST t
                // 4 ----
                // call subroutine in zero page memory
                case 0b111:
                  push(PC, cycles);
                  PC = 0x8 * regDest;
                  break;
              }
          }
          break;
      }
      break;
  }
}

// run the CB prefixed instruction specified
// by the given 8-bit opcode
void CPU::runInstrCB(uint8 opcode, uint8 &cycles) {
  uint8 upperTwoBits = (opcode >> 6) & 0b11;
  uint8 regDest = (opcode >> 3) & 0b111;
  uint8 regSrc = opcode & 0b111;

  switch (upperTwoBits) {
    case 0b00:
      switch (regDest) {
        // ==================================================
        // Rotate Shift Instructions
        // ==================================================

        // RLC r / RLC (HL)
        // 2/4 C00Z
        // rotate register r (or memory at HL)
        // to the left
        case 0b000:
          *regmap8[regSrc] = rotateLeft(*regmap8[regSrc]);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // RRC r / RRC (HL)
        // 2/4 C00Z
        // rotate register r (or memory at HL)
        // to the right
        case 0b001:
          *regmap8[regSrc] = rotateRight(*regmap8[regSrc]);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // RL r / RL (HL)
        // 2/4 C00Z
        // rotate register r (or memory at HL)
        // to the left through carry
        case 0b010:
          *regmap8[regSrc] = rotateLeft(*regmap8[regSrc], true);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // RR r / RR (HL)
        // 2/4 C00Z
        // rotate register r (or memory at HL)
        // to the right through carry
        case 0b011:
          *regmap8[regSrc] = rotateRight(*regmap8[regSrc], true);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // SLA r / SLA (HL)
        // 2/4 C00Z
        // shift register r (or memory at HL)
        // to the left
        case 0b100:
          *regmap8[regSrc] = shiftLeft(*regmap8[regSrc]);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // SRA r / SRA (HL)
        // 2/4 C00Z
        // shift register r (or memory at HL)
        // to the right arithmetically
        case 0b101:
          *regmap8[regSrc] = shiftRight(*regmap8[regSrc], true);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // SWAP r / SWAP (HL)
        // 2/4 000Z
        // swap lower and upper nibbles of
        // register r (or memory at HL)
        case 0b110:
          *regmap8[regSrc] = swap(*regmap8[regSrc]);
          if (regSrc == MEM_HL) cycles += 2;
          break;

        // SRL r / SRL (HL)
        // 2/4 C00Z
        // shift register r (or memory at HL)
        // to the right logically
        case 0b111:
          *regmap8[regSrc] = shiftRight(*regmap8[regSrc]);
          if (regSrc == MEM_HL) cycles += 2;
          break;
      }
      break;

    // ==================================================
    // Bit Operations
    // ==================================================

    // BIT b, r / BIT b, (HL)
    // 2/3 -10Z
    // copies complement of bit b of register r
    // (or memory at HL) into zero flag
    case 0b01:
      bit(*regmap8[regSrc], regDest);
      if (regSrc == MEM_HL) ++cycles;
      break;

    // RES b, r / RES b, (HL)
    // 2/4 ----
    // set bit b of register r (or memory at HL)
    // to zero
    case 0b10:
      *regmap8[regSrc] = set(*regmap8[regSrc], regDest, 0);
      if (regSrc == MEM_HL) cycles += 2;
      break;

    // SET b, r / BIT b, (HL)
    // 2/4 ----
    // set bit b of register r (or memory at HL)
    // to one
    case 0b11:
      *regmap8[regSrc] = set(*regmap8[regSrc], regDest, 1);
      if (regSrc == MEM_HL) cycles += 2;
      break;
  }
}

// **************************************************
// **************************************************
// Transfer Functions
// **************************************************
// **************************************************

// push the 16-bit number val onto the stack
void CPU::push(uint16 val, uint8 &cycles) {
  ++cycles;
  SP -= 2;
  mem.write(SP, val, cycles);
}

// pop from the stack and return popped
// 16-bit number
uint16 CPU::pop(uint8 &cycles) {
  uint16 val = mem.read16(SP, cycles);
  SP += 2;
  return val;
}

// **************************************************
// **************************************************
// Arithmetic and Logical Functions
// **************************************************
// **************************************************

// add 8-bit numbers a and b together, and
// optionally add the carry flag as well
//
// flags affected:
// C - true if carry, false otherwise
// H - true if carry from bit 3, false otherwise
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::add(uint8 a, uint8 b, bool c) {
  uint16 resultOverflow = a + b + c;
  uint8 result = resultOverflow & 0xFF;
  uint8 halfResult = (a & 0xF) + (b & 0xF) + c;
  carry = resultOverflow > 0xFF;
  halfCarry = halfResult > 0xF;
  subtract = false;
  zero = result == 0;
  return result;
}

// add 16-bit numbers a and b together
//
// flags affected:
// C - true if carry, false otherwise
// H - true if carry from bit 11, false otherwise
// N - set to false
uint16 CPU::add(uint16 a, uint16 b) {
  uint16 halfA = a & 0xFFF;
  uint16 result = a + b;
  uint16 halfResult = (halfA + (b & 0xFFF)) & 0xFFF;
  carry = result < a;
  halfCarry = halfResult < halfA;
  subtract = false;
  return result;
}

uint16 CPU::addSP(int8 val) {
  uint16 result = SP + val;
  if (val >= 0) {
    carry = ((SP & 0xFF) + val) > 0xFF;
    halfCarry = ((SP & 0xF) + (val & 0xF)) > 0xF;
  } else {
    carry = (result & 0xFF) < (SP & 0xFF);
    halfCarry = (result & 0xF) < (SP & 0xF);
  }
  subtract = false;
  zero = false;
  return SP + val;
}

// subtract 8-bit number b, and optionally carry,
// from 8-bit number a
//
// flags affected:
// C - true if borrow, false otherwise
// H - true if borrow from bit 4, false otherwise
// N - set to true
// Z - true if result is zero, false otherwise
uint8 CPU::sub(uint8 a, uint8 b, bool c) {
  uint8 result = a - b - c;
  carry = a < (b + c);
  halfCarry = (a & 0xF) < ((b & 0xF) + c);
  subtract = true;
  zero = result == 0;
  return result;
}

// and 8-bit number val with the accumulator
//
// flags affected:
// C - set to false
// H - set to true
// N - set to false
// Z - true if result is zero, false otherwise
void CPU::_and(uint8 val) {
  A &= val;
  carry = false;
  halfCarry = true;
  subtract = false;
  zero = A == 0;
}

// or 8-bit number val with the accumulator
//
// flags affected:
// C - set to false
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
void CPU::_or(uint8 val) {
  A |= val;
  carry = false;
  halfCarry = false;
  subtract = false;
  zero = A == 0;
}

// xor 8-bit number val with the accumulator
//
// flags affected:
// C - set to false
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
void CPU::_xor(uint8 val) {
  A ^= val;
  carry = false;
  halfCarry = false;
  subtract = false;
  zero = A == 0;
}

// **************************************************
// **************************************************
// Rotate Shift Functions
// **************************************************
// **************************************************

// rotate 8-bit number to the left, can optionally
// rotate through carry flag
//
// flags affected:
// C - set to bit 7 of val
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::rotateLeft(uint8 val, bool thruCarry) {
  bool bit7 = (val >> 7) & 0b1;
  uint8 thruBit = thruCarry ? carry : bit7;
  uint8 result = shiftLeft(val) | thruBit;
  zero = result == 0;
  return result;
}

// rotate 8-bit number to the right, can optionally
// rotate through carry flag
//
// flags affected:
// C - set to bit 0 of val
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::rotateRight(uint8 val, bool thruCarry) {
  bool bit0 = val & 0b1;
  uint8 thruBit = (thruCarry ? carry : bit0) << 7;
  uint8 result = shiftRight(val) | thruBit;
  zero = result == 0;
  return result;
}

// shift 8-bit number to the left, can optionally
// rotate through carry flag
//
// flags affected:
// C - set to bit 7 of val
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::shiftLeft(uint8 val) {
  bool bit7 = (val >> 7) & 0b1;
  val <<= 1;
  carry = bit7;
  halfCarry = false;
  subtract = false;
  zero = val == 0;
  return val;
}

// shift 8-bit number to the right, can choose
// to shift arithmetically or logically
//
// flags affected:
// C - set to bit 0 of val
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::shiftRight(uint8 val, bool arithmetic) {
  bool bit0 = val & 0b1;
  uint8 msb = arithmetic ? (val & 0x80) : 0;
  val = ((val >> 1) & 0x7F) | msb;
  carry = bit0;
  halfCarry = false;
  subtract = false;
  zero = val == 0;
  return val;
}

// swaps the upper and lower nibbles of the
// given 8-bit number val
//
// flags affected:
// C - set to false
// H - set to false
// N - set to false
// Z - true if result is zero, false otherwise
uint8 CPU::swap(uint8 val) {
  uint8 hiNibble = (val >> 4) & 0xF;
  uint8 loNibble = val & 0xF;
  uint8 result = (loNibble << 4) | hiNibble;
  carry = false;
  halfCarry = false;
  subtract = false;
  zero = result == 0;
  return result;
}

// **************************************************
// **************************************************
// Bit Functions
// **************************************************
// **************************************************

// copies complement of bit at specified
// position bitPos of the 8-bit number val to the
// zero flag
//
// flags affected:
// H - set to true
// N - set to false
// Z - set to complement of bit at bitpos
void CPU::bit(uint8 val, uint8 bitpos) {
  bool bit = (val >> bitpos) & 0b1;
  halfCarry = true;
  subtract = false;
  zero = !bit;
}

// set bit at specified position bitPos
// of 8-bit number val to the given bit
// value bitVal
uint8 CPU::set(uint8 val, uint8 bitPos, bool bitVal) {
  uint8 mask = ~(1 << bitPos);
  return val & mask | (bitVal << bitPos);
}

// **************************************************
// **************************************************
// Jump Functions
// **************************************************
// **************************************************

// return true if given jump condition is met
bool CPU::jumpCondMet(uint8 jumpCond) {
  switch (jumpCond) {
    case JUMP_NZ:
      return zero == false;
    case JUMP_Z:
      return zero == true;
    case JUMP_NC:
      return carry == false;
    case JUMP_C:
      return carry == true;
  }
  return false;
}

// **************************************************
// **************************************************
// General-Purpose Arithmetic Functions
// **************************************************
// **************************************************

// decimal adjust the accumulator after
// addition or subtraction with two
// binary coded decimals
void CPU::decimalAdjAcc() {
  uint8 correction = 0;
  if ((!subtract && (A & 0xF) > 0x9) || halfCarry) {
    correction += 0x6;
  }
  if ((!subtract && A > 0x99) || carry) {
    correction += 0x60;
    carry = true;
  }
  A += subtract ? -correction : correction;
  halfCarry = false;
  zero = A == 0;
}

// **************************************************
// **************************************************
// Register AF Setter + Getter
// **************************************************
// **************************************************

// get register AF, the accumulator
// combined with the flag register
uint16 CPU::getAF() {
  uint16 AF = A << 8;
  AF |= (zero << 7);
  AF |= (subtract << 6);
  AF |= (halfCarry << 5);
  AF |= (carry << 4);
  return AF;
}

// set register AF, the accumulator
// combined with the flag register
void CPU::setAF(uint16 val) {
  A = (val >> 8) & 0xFF;
  uint8 F = val & 0xFF;
  zero = (F >> 7) & 0b1;
  subtract = (F >> 6) & 0b1;
  halfCarry = (F >> 5) & 0b1;
  carry = (F >> 4) & 0b1;
}

// **************************************************
// **************************************************
// Interrupt Functions
// **************************************************
// **************************************************

void CPU::handleInterrupts(uint8 &cycles) {
  // resume running cpu from halt mode if there is an
  // interrupt that needs to be serviced
  if (halt && (mem.getByte(IF) & mem.getByte(IE) & 0x1F) != 0) {
    halt = false;
    cycles += 1;
  }

  if (IME) {
    uint16 intAddr = 0;
    if (Interrupts::requestedAndEnabled(V_BLANK_INT)) {
      intAddr = 0x40;
      Interrupts::reset(V_BLANK_INT);
    } else if (Interrupts::requestedAndEnabled(LCDC_INT)) {
      intAddr = 0x48;
      Interrupts::reset(LCDC_INT);
    } else if (Interrupts::requestedAndEnabled(TIMER_INT)) {
      intAddr = 0x50;
      Interrupts::reset(TIMER_INT);
    } else if (Interrupts::requestedAndEnabled(SERIAL_INT) || (mem.getByte(SC) & 0x81) == 0x81) {
      intAddr = 0x58;
      Interrupts::reset(SERIAL_INT);
    } else if (Interrupts::requestedAndEnabled(JOYPAD_INT)) {
      intAddr = 0x60;
      Interrupts::reset(JOYPAD_INT);
    }

    if (intAddr != 0) {
      IME = false;
      push(PC, cycles);
      PC = intAddr;
      cycles += 2;
    }
  }
}

// **************************************************
// **************************************************
// Log Function
// **************************************************
// **************************************************

void CPU::log(std::fstream &fs) {
  char logLine[256];
  snprintf(logLine, 256,
           "PC: %04x | SP: %04x | A: %02x | BC: %04x | DE: %04x | HL: %02x | "
           "IME: %d | LCDC: %02x | STAT: %02x | LY: %02x | SCX: %02x | SCY: "
           "%02x | IE: %02x | IF: %02x "
           "| CHNZ: %d%d%d%d\n",
           PC, SP, A, BC, DE, HL, IME, mem.getByte(LCDC), mem.getByte(STAT),
           mem.getByte(LY), mem.getByte(SCX), mem.getByte(SCY), mem.getByte(IE),
           mem.getByte(IF), carry, halfCarry, subtract, zero);
  fs.write(logLine, strlen(logLine));
}

void CPU::loadState() {
  std::fstream fs("/Users/paulscott/git/DotMatrix/debug/cpu_state.json",
                  std::ios::in);
  fs.seekg(0, std::ios::end);
  int size = fs.tellg();
  fs.seekg(0, std::ios::beg);

  char readBuf[size + 1];
  fs.read(readBuf, size);
  readBuf[size] = 0;

  auto state = nlohmann::json::parse(std::string(readBuf));
  PC = state["PC"];
  SP = state["SP"];
  A = state["A"];
  BC = state["BC"];
  DE = state["DE"];
  HL = state["HL"];
  carry = state["carry"];
  halfCarry = state["halfCarry"];
  subtract = state["subtract"];
  zero = state["zero"];
  IME = state["IME"];
  halt = state["halt"];
  stop = state["stop"];
}

void CPU::saveState() {
  nlohmann::json state = {
      {"PC", PC},       {"SP", SP},           {"A", A},
      {"BC", BC},       {"DE", DE},           {"HL", HL},
      {"carry", carry}, {"halfCarry", carry}, {"subtract", subtract},
      {"zero", zero},   {"IME", IME},         {"halt", halt},
      {"stop", stop}};
  std::string stateStr = state.dump(4);
  std::fstream fs("/Users/paulscott/git/DotMatrix/debug/cpu_state.json",
                  std::ios::out);
  fs.write(stateStr.c_str(), stateStr.size());
  fs.close();
}
