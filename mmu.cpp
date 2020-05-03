#include "mmu.h"
#include "ppu.h"

MMU::MMU() {
    mem = new unsigned char[0x10000];
    cart = new char[0x200000];
    ramEnabled = false;
    bankMode = true;
    bankType = NONE;
    bankUpperBits = 0;
    bankLowerBits = 0;

    // zero memory from 0x8000 to 0x9FFF
    for (int i = 0x8000; i < 0xA000; i++) {
        mem[i] = 0;
    }
}

// ================================
// Read from memory
// ================================
unsigned char MMU::read(unsigned short addr) const {

    // no banking
    if (bankType == NONE) {
        if (addr < 0x8000) {
            return (unsigned char)cart[addr];
        } else {
            return mem[addr];
        }
    }

    // MBC1
    else if (bankType == MBC1) {
        if (addr >= 0x0000 && addr < 0x4000) {
            return (unsigned char)cart[addr];
        } else if (addr >= 0x4000 && addr < 0x8000) {
            return (unsigned char)cart[bankUpperBits | bankLowerBits | (addr & 0x3FFF)];
        } else {
            return mem[addr];
        }
    }
    return 0;
}

// ================================
// Write to memory
// ================================
void MMU::write(unsigned short addr, unsigned char value) {
    checkForMBCRequest(addr, value);
    checkForDMATransfer(addr, value);
    checkForEcho(addr, value);

    // write value to address
    if (addr >= 0x8000) {
        if (addr == STAT) {
            mem[addr] = (mem[addr] & 0x87) | (value & 0x78);
        } else if (addr == DIVIDER) {
            mem[addr] = 0;
        } else {
            mem[addr] = value;
        }
    }

    // check for joypad input
    if (addr == JOYPAD) {
        checkForInput();
    }
}

void MMU::checkForMBCRequest(unsigned short addr, unsigned char value) {
    if (bankType == MBC1) {
        if (addr >= 0x0000 && addr < 0x2000) {
            ramEnabled = (value & 0xF) == 0xA;
        } else if (addr >= 0x2000 && addr < 0x4000) {
            bankLowerBits = (value & 0x1F) << 14;
            if (bankLowerBits == 0) {
                bankLowerBits = 0x4000;
            }
        } else if (addr >= 0x4000 && addr < 0x6000) {
            bankUpperBits = (value & 0x3) << 19;
        } else if (addr >= 0x6000 && addr < 0x8000) {
            bankMode = value & 0x1;
        }
    }
}

void MMU::checkForDMATransfer(unsigned short addr, unsigned char value) {
    if (addr == DMA) {
        unsigned short oamDataAddr = value << 8;
        for (int i = 0; i < OAM_COUNT * BYTES_PER_OAM; i++) {
            write(OAM_ADDR + i, read(oamDataAddr + i));
        }
    }
}

void MMU::checkForEcho(unsigned short addr, unsigned char value) const {
    if (addr >= 0xC000 && addr < 0xDE00) {
        mem[addr + 0x2000] = value;
    }
}

// ================================
// Load cartridge data into memory
// ================================
void MMU::loadCartridge(const std::string& dir) const {

    // read cartridge
    std::ifstream cartridge (dir, std::ios::in | std::ios::binary);
    cartridge.read(cart, 0x200000);
}

void MMU::checkForInput() {
    mem[JOYPAD] |= 0xCFu;
    if ((((unsigned char)(mem[JOYPAD] >> 4) & 0x1) == 0) || ((mem[JOYPAD] & 0x30) == 0x30)) {
        if (joypad[RIGHT]) {
            mem[JOYPAD] &= 0xFE;
        } else {
            mem[JOYPAD] |= 0x1;
        }

        if (joypad[LEFT]) {
            mem[JOYPAD] &= 0xFD;
        } else {
            mem[JOYPAD] |= 0x2;
        }

        if (joypad[UP]) {
            mem[JOYPAD] &= 0xFB;
        } else {
            mem[JOYPAD] |= 0x4;
        }

        if (joypad[DOWN]) {
            mem[JOYPAD] &= 0xF7;
        } else {
            mem[JOYPAD] |= 0x8;
        }
    } else if (((unsigned char)(mem[JOYPAD] >> 5) & 0x1) == 0) {
        if (joypad[BUTTON_A]) {
            mem[JOYPAD] &= 0xFE;
        } else {
            mem[JOYPAD] |= 0x1;
        }

        if (joypad[BUTTON_B]) {
            mem[JOYPAD] &= 0xFD;
        } else {
            mem[JOYPAD] |= 0x2;
        }

        if (joypad[SELECT]) {
            mem[JOYPAD] &= 0xFB;
        } else {
            mem[JOYPAD] |= 0x4;
        }

        if (joypad[START]) {
            mem[JOYPAD] &= 0xF7;
        } else {
            mem[JOYPAD] |= 0x8;
        }
    }
}
