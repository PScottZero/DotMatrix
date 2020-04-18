#include "mmu.h"

MMU::MMU() {
    mem = new unsigned char[0x10000];
    cart = new char[0x200000];

    // zero memory from 0x8000 to 0x9FFF
    for (int i = 0x8000; i < 0xA000; i++) {
        mem[i] = 0;
    }
}

// ================================
// Read from memory
// ================================
unsigned char MMU::read(unsigned short addr) const {
    return mem[addr];
}

// ================================
// Write to memory
// ================================
void MMU::write(unsigned short addr, unsigned char value) {

    // load bank
    if (addr >= 0x2000 && addr < 0x4000) {
        loadBank(value & 0x1F);
    }

    // perform dma transfer
    if (addr == DMA) {
        unsigned short oamDataAddr = value << 8;
        for (int i = 0; i < OAM_COUNT * BYTES_PER_OAM; i++) {
            write(OAM_ADDR + i, read(oamDataAddr + i));
        }
    }

        // echo ram
    else if (addr >= ECHO_START && addr < ECHO_END) {
        mem[WORK_RAM + addr - ECHO_START] = value;
    }

        // reset timer
    else if (addr == DIVIDER) {
        value = 0;
    }

    // write value to address
    if (addr > 0x7FFF) {
        mem[addr] = value;
    }

    // check for joypad input
    if (addr == JOYPAD) {
        checkForInput();
    }
}

// ================================
// Load cartridge data into memory
// ================================
void MMU::loadCartridge(const std::string& dir) const {

    // read cartridge
    std::ifstream cartridge (dir, std::ios::in | std::ios::binary);
    cartridge.read(cart, 0x200000);

    // put first 32kb of rom into ram
    for (unsigned short i = 0; i < 0x8000; i++) {
        mem[i] = (unsigned char)cart[i];
    }
}

// ================================
// Load cartridge bank
// ================================
void MMU::loadBank(unsigned char bankNo) const {
    if (bankNo == 0x00 || bankNo == 0x20 ||
        bankNo == 0x40 || bankNo == 0x60) {
        bankNo++;
    }
    unsigned int bankAddr = bankNo * BANK_SIZE;
    for (unsigned int i = 0; i < BANK_SIZE; i++) {
        mem[BANK_SIZE + i] = cart[bankAddr + i];
    }
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
