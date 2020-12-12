#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbThread.h"
#include <thread>

GBThread::GBThread(QObject *parent) : QThread(parent) {
    rom = "";
    emitted = false;
    ppu = new PPU(cpu.mmu->mem, &cpu.cycleCount);
    speed = 16;
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
// Set emulation speed
// ================================
void GBThread::setSpeed(int frameRate) {
    this->speed = frameRate;
}

// ================================
// Save RAM data
// ================================
void GBThread::saveRAM() const {
    cpu.mmu->saveRAM();
}

// ================================
// Run emulator thread
// ================================
void GBThread::run() {
    cpu.powerUp();
    QImage frame(160, 144, QImage::Format_RGB32);

    cpu.mmu->loadCartridge(rom);
    if (checkBankType()) {
        cpu.mmu->loadRAM();
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
                auto nextCycle = cycleStart + std::chrono::milliseconds(speed);
                std::this_thread::sleep_until(nextCycle);
                cycleStart = std::chrono::system_clock::now();
            } else if (cpu.mmu->read(LY) <= 143) {
                emitted = false;
            }
        }
    }
}

// ================================
// Check if bank type is supported
// ================================
bool GBThread::checkBankType() {
    unsigned char bankType = cpu.mmu->read(BANK_TYPE);
    if (bankType == 0x00) {
        cpu.mmu->bankType = NONE;
    } else if (bankType >= 0x01 && bankType <= 0x03) {
        cpu.mmu->bankType = MBC1;
    } else if (bankType >= 0x05 && bankType <= 0x06) {
        cpu.mmu->bankType = MBC2;
    } else if (bankType >= 0x0F && bankType <= 0x13) {
        cpu.mmu->bankType = MBC3;
    } else if (bankType >= 0x19 && bankType <= 0x1E) {
        cpu.mmu->bankType = MBC5;
    } else {
        emit sendBankError(bankType);
        return false;
    }
    cpu.mmu->hasRAM = checkForRAM(bankType);
    cpu.mmu->hasBattery = checkForBattery(bankType);
    return true;
}

// ================================
// Banking methods with RAM
// ================================
bool GBThread::checkForRAM(unsigned char bankType) {
    return bankType == 0x02 || bankType == 0x03 ||
        bankType == 0x05 || bankType == 0x06 ||
        bankType == 0x10 || bankType == 0x12 ||
        bankType == 0x13 || bankType == 0x1A ||
        bankType == 0x1B || bankType == 0x1D ||
        bankType == 0x1E;
}

// ================================
// Banking methods with battery
// ================================
bool GBThread::checkForBattery(unsigned char bankType) {
    return bankType == 0x03 || bankType == 0x06 ||
        bankType == 0x0F || bankType == 0x10 ||
        bankType == 0x13 || bankType == 0x1B ||
        bankType == 0x1E;
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