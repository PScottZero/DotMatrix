#include "mmu.h"
#include "ppu.h"

MMU::MMU() {
    mem = new unsigned char[GB_MEM_SIZE];
    cart = new char[CART_SIZE];
    ram = new char[EXT_RAM_SIZE];
    ramEnabled = false;
    hasRAM = false;
    bankMode = true;
    bankType = NONE;
    bankUpperBits = 0;
    bankLowerBits = 1;
    ramBank = 0;

    // zero memory from 0x8000 to 0x9FFF
    for (int i = VRAM_START; i < VRAM_END; i++) {
        mem[i] = 0;
    }

    // zero RAM
    for (unsigned int i = 0; i < EXT_RAM_SIZE; i++) {
        ram[i] = 0;
    }
}

// ================================
// Read from memory
// ================================
unsigned char MMU::read(unsigned short addr) const {

    // no banking
    if (bankType == NONE) {

        // read from rom banks 0 and 1
        if (addr < ROM_BANK_1_END) {
            return (unsigned char)cart[addr];
        }

        // read from regular memory
        else {
            return mem[addr];
        }
    }

    // MBC
    else {

        // read from rom bank 0
        if (addr >= ROM_BANK_0_START && addr < ROM_BANK_0_END) {
            return cart[addr];
        }

        // read from rom bank 1
        else if (addr >= ROM_BANK_1_START && addr < ROM_BANK_1_END) {
            return cart[(bankUpperBits << 19) | (bankLowerBits << 14) | (addr & 0x3FFF)];
        }

        // read from external ram
        else if (addr >= EXT_RAM_START && addr < EXT_RAM_END) {
            if (hasRAM && ramEnabled) {
                if (bankType == MBC2) {
                    return ram[addr & 0x1FF];
                } else if (ramBank < 0x4) {
                    return ram[(ramBank << 13) | (addr & 0x1FFF)];
                }
                return 0;
            }
        }

        // read from regular memory
        else {
            return mem[addr];
        }
    }
}

// ================================
// Write to memory
// ================================
void MMU::write(unsigned short addr, unsigned char value) {
    checkForMBCRequest(addr, value);
    checkForDMATransfer(addr, value);
    checkForEcho(addr, value);

    // write value to address
    if (addr >= ROM_BANK_1_END) {

        // write to lcd stat register
        if (addr == STAT) {
            mem[addr] = (mem[addr] & 0x87) | (value & 0x78);
        }

        // powerUp divider register
        else if (addr == DIV) {
            mem[addr] = 0;
        }

        // write to external ram
        else if (addr >= EXT_RAM_START && addr < EXT_RAM_END) {
            if (hasRAM && ramEnabled) {
                if (bankType == MBC2) {
                    ram[addr & 0x1FF] = (char) (value & 0xF);
                } else if (ramBank < 0x4) {
                    ram[(ramBank << 13) | (addr & 0x1FFF)] = (char) value;
                }
            }
        }

        // write to regular memory
        else {
            mem[addr] = value;

            if (addr == IF) {
                mem[addr] |= 0xE0;
            }
        }
    }

    // check for joypad input
    if (addr == JOYP) {
        checkForInput();
    }
}

// ================================
// Check if bank setting is
// being changed
// ================================
void MMU::checkForMBCRequest(unsigned short addr, unsigned char value) {

    // ram enable
    if (addr >= 0x0000 && addr < 0x2000) {
        ramEnabled = (value & 0xF) == 0xA;
    }

    // ========================
    // MBC1 BANK TYPE
    // ========================
    if (bankType == MBC1) {

        // lower five bits of bank number
        if (addr >= 0x2000 && addr < 0x4000) {
            bankLowerBits = (value & 0x1F);
            if (bankLowerBits == 0) {
                bankLowerBits = 0x1;
            }
        }

        // upper two bits of bank number
        // or ram bank number
        else if (addr >= 0x4000 && addr < 0x6000) {
            if (bankMode == 0) {
                bankUpperBits = (value & 0x3);
            } else {
                ramBank = (value & 0x3);
            }
        }

        // bank mode
        else if (addr >= 0x6000 && addr < 0x8000) {
            bankMode = value & 0x1;
        }
    }

    // ========================
    // MBC2 BANK TYPE
    // ========================
    else if (bankType == MBC2) {

        // bank number
        if (addr >= 0x2000 && addr < 0x4000) {
            bankLowerBits = (value & 0xF);
            bankUpperBits = 0;
            if (bankLowerBits == 0) {
                bankLowerBits = 0x1;
            }
        }
    }

    // ========================
    // MBC3 BANK TYPE
    // ========================
    else if (bankType == MBC3) {

        // bank number
        if (addr >= 0x2000 && addr < 0x4000) {
            bankLowerBits = value & 0x7F;
            bankUpperBits = 0;
            if (value == 0) {
                bankLowerBits = 0x1;
            }
        }

        // ram bank number
        else if (addr >= 0x4000 && addr < 0x6000) {
            ramBank = (value & 0xF);
        }
    }
}

// ================================
// Perform DMA transfer if address
// is equal to 0xFF46
// ================================
void MMU::checkForDMATransfer(unsigned short addr, unsigned char value) {
    if (addr == DMA) {
        unsigned short oamDataAddr = value << 8;
        for (int i = 0; i < OAM_COUNT * BYTES_PER_OAM; i++) {
            write(OAM_ADDR + i, read(oamDataAddr + i));
        }
    }
}

// ================================
// Write to echo ram if addr
// is in range
// ================================
void MMU::checkForEcho(unsigned short addr, unsigned char value) const {
    if (addr >= WORK_RAM_START && addr < WORK_RAM_ECHO_END) {
        mem[addr + ECHO_OFFSET] = value;
    }
}

// ================================
// Load cartridge data into memory
// ================================
void MMU::loadCartridge(std::string dir) {
    cartDir = std::move(dir);;
    std::ifstream cartridge (cartDir, std::ios::in | std::ios::binary);
    cartridge.read(cart, CART_SIZE);
    cartridge.close();
}

// ================================
// Load RAM data
// ================================
void MMU::loadRAM() const {
    if (hasRAM) {
        std::string ramDir = cartDir.substr(0, cartDir.size() - 3) + ".exram";
        std::ifstream ramBuffer (ramDir, std::ios::in | std::ios::binary);
        if (!ramBuffer.fail()) {
            ramBuffer.read(ram, EXT_RAM_SIZE);
        }
        ramBuffer.close();
    }
}

// ================================
// Save RAM data
// ================================
void MMU::saveRAM() const {
    if (hasRAM) {
        std::string ramDir = cartDir.substr(0, cartDir.size() - 3) + ".exram";
        std::ofstream saveData (ramDir, std::ios::out | std::ios::binary);

        // determine save size
        unsigned int saveSize = 0;
        for (unsigned int i = EXT_RAM_SIZE - 1; i >= 0; i--) {
            if (ram[i] != 0) {
                saveSize = i + 1;
                break;
            }
        }
        saveData.write(ram, saveSize);
        saveData.close();
    }
}

// ================================
// Check for joypad input
// ================================
void MMU::checkForInput() {
    mem[JOYP] |= 0xCFu;
    if ((((unsigned char)(mem[JOYP] >> 4) & 0x1) == 0) || ((mem[JOYP] & 0x30) == 0x30)) {
        if (joypad[RIGHT]) {
            mem[JOYP] &= 0xFE;
        } else {
            mem[JOYP] |= 0x1;
        }

        if (joypad[LEFT]) {
            mem[JOYP] &= 0xFD;
        } else {
            mem[JOYP] |= 0x2;
        }

        if (joypad[UP]) {
            mem[JOYP] &= 0xFB;
        } else {
            mem[JOYP] |= 0x4;
        }

        if (joypad[DOWN]) {
            mem[JOYP] &= 0xF7;
        } else {
            mem[JOYP] |= 0x8;
        }
    } else if (((unsigned char)(mem[JOYP] >> 5) & 0x1) == 0) {
        if (joypad[BUTTON_A]) {
            mem[JOYP] &= 0xFE;
        } else {
            mem[JOYP] |= 0x1;
        }

        if (joypad[BUTTON_B]) {
            mem[JOYP] &= 0xFD;
        } else {
            mem[JOYP] |= 0x2;
        }

        if (joypad[SELECT]) {
            mem[JOYP] &= 0xFB;
        } else {
            mem[JOYP] |= 0x4;
        }

        if (joypad[START]) {
            mem[JOYP] &= 0xF7;
        } else {
            mem[JOYP] |= 0x8;
        }
    }
}
