#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbThread.h"
#include <thread>
#include <utility>

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    rom = "";
    emitted = false;
    ppu = new PPU(cpu.mmu->mem, &cpu.cycleCount);
}

GBThread::~GBThread() {
    delete[] cpu.mmu->mem;
    delete[] cpu.mmu->cart;
    terminate();
    wait();
}

void GBThread::setRom(std::string dir) {
    rom = std::move(dir);
}

void GBThread::run() {
    cpu.reset();
    QImage frame(160, 144, QImage::Format_RGB32);

    cpu.mmu->loadCartridge(rom);
    ppu->setDisplay(&frame);

    auto cycleStart = std::chrono::system_clock::now();

    forever {
        cpu.step();
        ppu->step();

        // send frame to widget
        if (cpu.mmu->mem[LY] > 143 && !emitted) {
            emitted = true;
            if (!ppu->lcdEnable()) {
                frame.fill(0xe0f0e8);
            }
            emit sendFrame(frame);
            auto nextCycle = cycleStart + std::chrono::milliseconds(16);
            std::this_thread::sleep_until(nextCycle);
            cycleStart = std::chrono::system_clock::now();
        } else if (cpu.mmu->mem[LY] <= 143) {
            emitted = false;
        }
    }
}

void GBThread::processInput(Joypad button, bool pressed) {
    bool oldState = cpu.mmu->joypad[button];
    cpu.mmu->joypad[button] = pressed;
    if (!oldState && cpu.mmu->joypad[button]) {
        cpu.mmu->mem[IF] |= 0x10;
    }
}

#pragma clang diagnostic pop