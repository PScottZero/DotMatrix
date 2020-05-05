#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbThread.h"

GBThread::GBThread(QObject *parent) : QThread(parent) {
    rom = "";
    emitted = false;
    ppu = new PPU(cpu.mmu->mem, &cpu.cycleCount);
}

GBThread::~GBThread() {
    cpu.mmu->saveRAM();
    delete[] cpu.mmu->ram;
    delete[] cpu.mmu->mem;
    delete[] cpu.mmu->cart;
    terminate();
    wait();
}

// ================================
// Set ROM path
// ================================
void GBThread::setRom(std::string dir) {
    rom = std::move(dir);
}

// ================================
// Save RAM data
// ================================
void GBThread::saveRAM() {
    cpu.mmu->saveRAM();
}

// ================================
// Run emulator thread
// ================================
void GBThread::run() {
    cpu.reset();
    QImage frame(160, 144, QImage::Format_RGB32);

    cpu.mmu->loadCartridge(rom);
    cpu.mmu->loadRAM();
    if (checkBankType()) {
        ppu->setDisplay(&frame);
        auto cycleStart = std::chrono::system_clock::now();

        forever {
            // step through machine cycle
            cpu.step();
            ppu->step();

            // send frame to widget
            if (cpu.mmu->read(LY) > 143 && !emitted) {
                emitted = true;
                if (!ppu->lcdEnable()) {
                    frame.fill(ppu->palette->getColor(PX_ZERO));
                }
                emit sendFrame(frame);
                auto nextCycle = cycleStart + std::chrono::milliseconds(16);
                std::this_thread::sleep_until(nextCycle);
                cycleStart = std::chrono::system_clock::now();
            } else if (cpu.mmu->read(LY) <= 143) {
                emitted = false;
            }
        }
    }
}

// ================================
// Check if bank type is valid
// ================================
bool GBThread::checkBankType() {
    unsigned char bankType = cpu.mmu->read(BANK_TYPE);
    if (bankType == 0x00) {
        printf("NON-BANKING ROM\n");
        cpu.mmu->bankType = NONE;
    } else if (bankType >= 0x01 && bankType <= 0x03) {
        printf("MBC1 ROM\n");
        cpu.mmu->bankType = MBC1;
    } else if (bankType >= 0x05 && bankType <= 0x06) {
        printf("MBC2 ROM\n");
        cpu.mmu->bankType = MBC2;
    } else if (bankType >= 0x0F && bankType <= 0x13) {
        printf("MBC3 ROM\n");
        cpu.mmu->bankType = MBC3;
    } else {
        emit sendBankError(bankType);
        return false;
    }
    return true;
}

// ================================
// Process input from
// DMWindow class
// ================================
void GBThread::processInput(Joypad button, bool pressed) {
    bool oldState = cpu.mmu->joypad[button];
    cpu.mmu->joypad[button] = pressed;
    if (!oldState && cpu.mmu->joypad[button]) {
        cpu.setInt(JOYPAD_PRESSED);
    }
}

#pragma clang diagnostic pop