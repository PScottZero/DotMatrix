#include "mmu.h"

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
    if (addr >= 0x0000 && addr < 0x4000) {
        return cart[addr];
    } else if (addr >= 0x4000 && addr < 0x8000) {
        if (bankType == NONE) {
            return cart[addr];
        } else {
            return cart[bankUpperBits | bankLowerBits | (addr & 0x3FFF)];
        }
    } else {
        return mem[addr];
    }
}

// ================================
// Write to memory
// ================================
void MMU::write(unsigned short addr, unsigned char value) {
    if (addr == DIVIDER) {
        printf("yes\n");
    }

    checkForMBCRequest(addr, value);
    checkForDMATransfer(addr, value);
    checkForEcho(addr, value);

    // reset timer
    if (addr == DIVIDER) {
        value = 0;
    }

    // write value to address
    if (addr >= 0x8000) {
        mem[addr] = value;
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

    // put first 32kb of rom into ram
    for (unsigned short i = 0; i < 0x8000; i++) {
        mem[i] = (unsigned char)cart[i];
    }
}

// ================================
// Load cartridge bank
// ================================
void MMU::loadROMBank(unsigned char bankLowerBits) const {
    if (bankType == MBC1) {
        if (bankLowerBits == 0x0) {
            bankLowerBits = 0x1;
        }
    }
    unsigned char bankNo = bankUpperBits | bankLowerBits;
    unsigned int bankAddr = bankNo * BANK_SIZE;
    for (unsigned int i = 0; i < BANK_SIZE; i++) {
        mem[BANK_SIZE + i] = cart[bankAddr + i];
    }
}

void MMU::loadRAMBank(unsigned char) const {
    std::cout << "RAM BANKING NOT IMPLEMENTED YET" << std::endl;
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
