// **************************************************
// **************************************************
// **************************************************
// Memory (Cartridge, VRAM, WRAM, etc.)
// **************************************************
// **************************************************
// **************************************************

#include "memory.h"

#include "bootstrap.h"
#include "cgb.h"
#include "cpu.h"
#include "interrupts.h"
#include "log.h"
#include "mbc.h"

// **************************************************
// **************************************************
// Memory Map:
//
// 0000-3FFF -> rom bank 0
// 4000-7FFF -> rom bank 1-N
// 8000-9FFF -> video ram bank (vram)
// A000-BFFF -> external ram bank (exram)
// C000-CFFF -> work ram bank 0 (wram)
// D000-DFFF -> work ram bank 1-7 (wram)
// E000-FDFF -> echo ram (mirror of C000-DDFF)
// FE00-FE9F -> sprite attribute table (oam)
// FEA0-FEFF -> not usable
// FF00-FF7F -> i/o registers
// FF80-FFFE -> high ram (hram)
// FFFF-FFFF -> interrupt enable register
// **************************************************
// **************************************************

Memory::Memory()
    : cgb(nullptr),
      mem((uint8 *)malloc(MEM_BYTES)),
      cart((uint8 *)malloc(CART_BYTES)),
      vram((uint8 *)malloc(RAM_BANK_BYTES * VRAM_BANKS)),
      exram((uint8 *)malloc(RAM_BANK_BYTES * EXRAM_BANKS)),
      wram((uint8 *)malloc(WRAM_BANK_BYTES * WRAM_BANKS)),
      cramBg{},
      cramObj{},
      romBank0(&cart[0]),
      romBank1(&cart[ROM_BANK_BYTES]),
      vramBank(&vram[0]),
      exramBank(&exram[0]),
      wramBank(&wram[WRAM_BANK_BYTES]),
      bcpd(&cramBg[0]),
      ocpd(&cramObj[0]) {}

// **************************************************
// **************************************************
// Read + Write Functions
// **************************************************
// **************************************************

// read 8-bit value from given memory address
uint8 Memory::read(uint16 addr) const {
  cgb->cpu.ppuTimerSerialStep(1);

  // **************************************************
  // Read Intercepts
  // **************************************************

  // bootstrap
  if (cgb->bootstrap.enabled) {
    bool dmgBoot = addr < DMG_BOOTSTRAP_BYTES;
    bool cgbBoot = cgb->cgbMode && addr >= CGB_BOOTSTRAP_ADDR &&
                   addr < CGB_BOOTSTRAP_BYTES;
    if (dmgBoot || cgbBoot) return *cgb->bootstrap.at(addr);
  }

  // external ram access
  if (addr >= EXRAM_ADDR && addr < WRAM_ADDR) {
    // cannot access external ram if it
    // is not enabled by mbc
    if (!cgb->mbc.ramEnabled) return 0xFF;

    // if using mbc3 and external ram is
    // mapped to an rtc register, read
    // from rtc register
    if (cgb->mbc.usingMbc3() && cgb->mbc.ramBankNum >= RTC_REG_START_IDX) {
      return *cgb->rtc.rtcReg;
    }
  }

  // do not read value if reading from
  // write-only memory register
  if (addr == NR13 || addr == NR23 || addr == NR31 || addr == NR33 ||
      addr == NR41 || addr == HDMA1 || addr == HDMA2 || addr == HDMA3 ||
      addr == HDMA4) {
    return 0xFF;
  }

  // only the upper 2-bits of register
  // NR11 and NR21 can be read
  if (addr == NR11 || addr == NR21) {
    return readBits(addr, {6, 7});
  }

  // only bit 6 of register NR14,
  // NR24, NR34 and NR44 can be read
  if (addr == NR14 || addr == NR24 || addr == NR34 || addr == NR44) {
    return readBits(addr, {6});
  }

  // read from game boy color palette data
  if (cgb->cgbMode && addr == BCPD) return *bcpd;
  if (cgb->cgbMode && addr == OCPD) return *ocpd;

  // skip waiting for screen frame in
  // bootstrap by returning hex 90 when
  // reading the LY register
  if (addr == LY && cgb->bootstrap.skipDmgBootstrap()) {
    return 0x90;
  }

  // **************************************************
  // Read From Memory
  // **************************************************
  return getByte(addr);
}

// read 16-bit value from given memory address
uint16 Memory::read16(uint16 addr) const {
  return read(addr) | (read(addr + 1) << 8);
}

// only read the specified bits of the given
// 8-bit value at the given memory address,
// all other bits are set to 1
uint8 Memory::readBits(uint16 addr, vector<uint8> bits) const {
  uint8 mask = 0xFF;
  for (uint8 bit : bits) mask ^= (uint8)pow(2, bit);
  return getByte(addr) | mask;
}

// write 8-bit value to given memory address
void Memory::write(uint16 addr, uint8 val) {
  cgb->cpu.ppuTimerSerialStep(1);

  // **************************************************
  // Write Intercepts
  // **************************************************

  // check for mbc writes
  cgb->mbc.write(addr, val);

  // rom bank area is read-only
  if (addr < VRAM_ADDR) return;

  // external memory write
  if (addr >= EXRAM_ADDR && addr < WRAM_ADDR) {
    // cannot access external ram if it
    // is not enabled by MBC
    if (!cgb->mbc.ramEnabled) return;

    // if using mbc2, ram only uses lower nibble
    // of each ram byte, and ram only consists of
    // 512 half bytes with rest of ram area
    // echoing A000-A1FF
    if (cgb->mbc.usingMbc2()) {
      echoHalfRam(addr, val);
      return;
    }

    // if using mbc3 and external ram is
    // mapped to an rtc register, write
    // to rtc register
    if (cgb->mbc.usingMbc3() && cgb->mbc.ramBankNum >= RTC_REG_START_IDX) {
      *cgb->rtc.rtcReg = val;
      cgb->rtc.resetClock();
      return;
    }
  }

  // do not write value if writing to
  // read-only memory register
  if (addr == LY || addr == PCM12 || addr == PCM34) return;

  // only bits 4 and 5 of register P1
  // can be written to
  if (addr == P1) {
    writeBits(addr, val, {4, 5});
  }

  // writing anything to DIV register
  // will reset the internal counter
  // and the DIV register
  if (addr == DIV) {
    cgb->timers.reset();
    return;
  }

  // only bit 7 of register NR52 can
  // be written to
  if (addr == NR52) {
    writeBits(addr, val, {7});
    return;
  }

  // only bits 3-6 of the STAT register
  // can be written to
  if (addr == STAT) {
    writeBits(addr, val, {3, 4, 5, 6});
    return;
  }

  // prepare speed switch, only
  // bit 0 of register KEY1 can
  // be written to
  if (addr == KEY1) {
    writeBits(addr, val, {0});
    if (val & BIT0_MASK) cgb->doubleSpeedMode = !cgb->doubleSpeedMode;
    writeBits(addr, cgb->doubleSpeedMode << 7, {7});
    return;
  }

  // can write to all bits of register RP
  // except for bit 1
  if (addr == RP) {
    writeBits(addr, val, {0, 2, 3, 4, 5, 6, 7});
    return;
  }

  // only lower 5 bits of interrupt flag
  // register can be written to (top 3
  // bits are always 1)
  if (addr == IF) {
    getByte(addr) = val | 0xE0;
    return;
  }

  // echo ram
  if (addr >= WRAM_ADDR && addr < OAM_ADDR) {
    echoRam(addr, val);
    return;
  }

  // write to cgb background palette data (cgb only)
  if (cgb->cgbMode && addr == BCPD) {
    *bcpd = val;

    // auto increment register BCPS if
    // writing to register BCPD and auto
    // increment is enabled (cgb only)
    if (getByte(BCPS) & BIT7_MASK) {
      ++getByte(BCPS);
      getByte(BCPS) &= 0xBF;
      bcpd = &cramBg[getByte(BCPS) & SIX_BITS_MASK];
    }
  }

  // write to cgb object palette data (cgb only)
  if (cgb->cgbMode && addr == OCPD) {
    *ocpd = val;

    // auto increment register OCPS if
    // writing to register OCPD and auto
    // increment is enabled
    if (getByte(OCPS) & BIT7_MASK) {
      ++getByte(OCPS);
      getByte(OCPS) &= 0xBF;
      ocpd = &cramObj[getByte(OCPS) & SIX_BITS_MASK];
    }
  }

  // **************************************************
  // Write To Memory
  // **************************************************
  getByte(addr) = val;

  // **************************************************
  // Post-Write Actions
  // **************************************************

  // start oam dma transfer if DMA
  // register was written to
  if (addr == DMA) {
    oamDmaTransfer();
  }

  if (cgb->cgbMode &&)

    // start vram dma transfer if HDMA5
    // register was written to (cgb only)
    if (cgb->cgbMode && addr == HDMA5) {
      vramDmaTransfer();
      getByte(HDMA1) = 0xFF;
      getByte(HDMA2) = 0xFF;
      getByte(HDMA3) = 0xFF;
      getByte(HDMA4) = 0xFF;
      getByte(HDMA5) = 0xFF;
    }

  // turn off bootstrap if writing
  // nonzero value to address FF50
  if (addr == BOOTSTRAP && val != 0) {
    cgb->bootstrap.enabled = false;
    Log::bootstrap = false;

    // set dmg compatibility mode if
    // running an original game boy
    // game on the game boy color
    //
    // cgbMode | dmgMode | device
    // ----------------------------------
    // false   | true    | DMG
    // true    | true    | CGB (DMG mode)
    // true    | false   | CGB
    if (cgb->cgbMode) {
      cgb->dmgMode = !(cgb->mem.getByte(CGB_MODE) == 0x80 ||
                       cgb->mem.getByte(CGB_MODE) == 0xC0);
    }
  }

  // start serial transfer if writing
  // to register SC with bit 7 and bit
  // 0 being set to 1
  if (addr == SC && (val & (BIT7_MASK | BIT0_MASK)) == 0x81) {
    cgb->cpu.serialTransferMode = true;
  }

  // set vram bank if writing to
  // VBK register (cgb only)
  if (cgb->cgbMode && addr == VBK) {
    setVramBank(val & BIT0_MASK);
  }

  // set wram bank if writing to
  // SVBK register (cgb only)
  if (cgb->cgbMode && addr == SVBK) {
    setWramBank(max(val & THREE_BITS_MASK, 1));
  }

  // udpate register BCPD when writing
  // to BCPS (cgb only)
  if (cgb->cgbMode && addr == BCPS) {
    uint8 cramAddr = val & SIX_BITS_MASK;
    bcpd = &cramBg[cramAddr];
  }

  // udpate register OCPD when writing
  // to OCPS (cgb only)
  if (cgb->cgbMode && addr == OCPS) {
    uint8 cramAddr = val & SIX_BITS_MASK;
    ocpd = &cramObj[cramAddr];
  }
}

// write 16-bit value to given memory address
void Memory::write(uint16 addr, uint16 val) {
  write(addr, (uint8)(val & BYTE_MASK));
  write(addr + 1, (uint8)((val >> 8) & BYTE_MASK));
}

// only write the specified bits of the given
// 8-bit value to the given memory address
void Memory::writeBits(uint16 addr, uint16 val, vector<uint8> bits) {
  uint8 mask = 0x00;
  for (uint8 bit : bits) mask |= (uint8)pow(2, bit);
  getByte(addr) = (getByte(addr) & ~mask) | (val & mask);
}

// get 8-bit immediate value
uint8 Memory::imm8(uint16 &PC) const { return read(PC++); }

// get 16-bit immediate value
uint16 Memory::imm16(uint16 &PC) const {
  uint16 val = read16(PC++);
  ++PC;
  return val;
}

// get byte at specified address directly
// (no read/write intercepts)
uint8 &Memory::getByte(uint16 addr) const { return *getBytePtr(addr); }

// get pointer to byte at specified address
// directly (no read/write intercepts)
uint8 *Memory::getBytePtr(uint16 addr) const {
  if (addr < ROM_BANK_1_ADDR) {
    return &romBank0[addr];
  } else if (addr < VRAM_ADDR) {
    return &romBank1[addr - ROM_BANK_1_ADDR];
  } else if (addr < EXRAM_ADDR) {
    return &vramBank[addr - VRAM_ADDR];
  } else if (addr < WRAM_ADDR) {
    return &exramBank[addr - EXRAM_ADDR];
  } else if (addr < WRAM_BANK_ADDR) {
    return &wram[addr - WRAM_ADDR];
  } else if (addr < ECHO_RAM_ADDR) {
    return &wramBank[addr - WRAM_BANK_ADDR];
  }
  return &mem[addr - ECHO_RAM_ADDR];
}

// get byte from video ram, if bank if false
// the get byte from vram bank 0, if bank is
// true get byte from vram bank 1
uint8 &Memory::getVramByte(uint16 addr, bool bank) const {
  return vram[addr - VRAM_ADDR + (bank ? RAM_BANK_BYTES : 0)];
}

// **************************************************
// **************************************************
// Miscellaneous Functions
// **************************************************
// **************************************************

// load rom at the given directory into memory
void Memory::loadRom(QString dir) {
  fstream fs(dir.toStdString());
  fs.read((char *)cart, CART_BYTES);
  fs.close();

  // set default memory banks
  setRomBank(&romBank0, 0);
  setRomBank(&romBank1, 1);
  setVramBank(0);
  setExramBank(0);
  setWramBank(1);
}

// map memory rom bank to cartridge rom bank
void Memory::setRomBank(uint8 **romBank, uint8 bankNum) {
  *romBank = &cart[ROM_BANK_BYTES * bankNum];
}

// set video ram bank (cgb only)
void Memory::setVramBank(uint8 bankNum) {
  vramBank = &vram[RAM_BANK_BYTES * bankNum];
}

// set external ram bank
void Memory::setExramBank(uint8 bankNum) {
  exramBank = &exram[RAM_BANK_BYTES * bankNum];
}

// set work ram bank
void Memory::setWramBank(uint8 bankNum) {
  wramBank = &wram[WRAM_BANK_BYTES * bankNum];
}

// get vram transfer mode, true means
// hblank dma, false means general dma
bool Memory::vramTransferMode() const { return getByte(HDMA5) & BIT7_MASK; }

// get length of vram transfer
uint16 Memory::vramTransferLength() const {
  return ((getByte(HDMA5) & ~BIT7_MASK) + 1) * 0x10;
}

// perform oam dma transfer
void Memory::oamDmaTransfer() {
  uint16 dmaAddr = getByte(DMA) << 8;
  for (int i = 0; i < _OAM_ENTRY_COUNT * _OAM_ENTRY_BYTES; ++i) {
    getByte(OAM_ADDR + i) = (uint8)getByte(dmaAddr + i);
  }
}

// perform vram dma transfer
void Memory::vramDmaTransfer() {
  uint16 vramDmaSrc = (getByte(HDMA1) << 8) | (getByte(HDMA2) & ~NIBBLE_MASK);
  uint16 vramDmaDest = VRAM_ADDR | ((getByte(HDMA3) & FIVE_BITS_MASK) << 8) |
                       (getByte(HDMA4) & ~NIBBLE_MASK);

  printf("\n===== %s =====\n", vramTransferMode() ? "HDMA" : "GDMA");
  printf("SRC:        %04X\n", vramDmaSrc);
  printf("DEST:       %04X\n", vramDmaDest);
  printf("LENGTH:      %03X\n", vramTransferLength());
  printf("VRAM BANK:     %d\n", getByte(VBK) & BIT0_MASK);
  printf("WRAM BANK:     %d\n", getByte(SVBK) & THREE_BITS_MASK);
  printf("ROM BANK 1:  %03X\n", cgb->mbc.mbc5RomBankNum());

  for (int i = 0; i < vramTransferLength(); ++i) {
    getByte(vramDmaDest) = (uint8)getByte(vramDmaSrc);
    vramDmaSrc++;
    vramDmaDest++;
  }
}

// echo memory range C000-DDFF in memory
// range E000-FDFF
void Memory::echoRam(uint16 addr, uint8 val) {
  uint16 offset = addr % RAM_BANK_BYTES;
  getByte(WRAM_ADDR + offset) = val;
  if (ECHO_RAM_ADDR + offset < OAM_ADDR) {
    getByte(ECHO_RAM_ADDR + offset) = val;
  }
}

// echo half-ram memory range A000-A1FF in the
// 15 equal size memory ranges in the rest of
// of the external ram bank
void Memory::echoHalfRam(uint16 addr, uint8 val) {
  uint16 offset = addr % HALF_RAM_BYTES;
  for (int echoAddr = EXRAM_ADDR; echoAddr < WRAM_ADDR;
       echoAddr += HALF_RAM_BYTES) {
    getByte(echoAddr + offset) = val | 0xF0;
  }
}

// load external ram, should have same name
// and be in the same directory as the rom,
// except with a .sav extension
void Memory::loadExram() {
  string exramPath = cgb->romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::in);
  if (!fs.fail()) fs.read((char *)exram, cgb->mbc.ramBytes());
}

// save external ram, will be saved in the same
// directory as the rom and have the same name
// except with a .sav extension
void Memory::saveExram() {
  string exramPath = cgb->romPath.toStdString();
  exramPath.replace(exramPath.find(".gb"), 4, ".sav");
  fstream fs(exramPath, ios::out);
  fs.write((char *)exram, cgb->mbc.ramBytes());
}

// reset memory
void Memory::reset() {
  // clear memory
  for (int i = 0; i < MEM_BYTES; ++i) mem[i] = 0;

  // save external ram if current game
  // has ram and battery
  if (cgb->mbc.hasRamAndBattery()) saveExram();

  // clear external ram
  for (int i = 0; i < RAM_BANK_BYTES * EXRAM_BANKS; ++i) exram[i] = 0;

  // set upper three bits of interrupt
  // flag register
  *cgb->interrupts.intFlags = 0xE0;

  // reset rom and ram banks
  setRomBank(&romBank0, 0);
  setRomBank(&romBank1, 1);
  setVramBank(0);
  setExramBank(0);
  setWramBank(1);
  bcpd = &cramBg[0];
  ocpd = &cramObj[0];
}
