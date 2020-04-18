#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbthread.h"
#include <thread>

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    emitted = false;
}

GBThread::~GBThread() {
    delete[] cpu.mmu->mem;
    delete[] cpu.mmu->cart;
};

void GBThread::run() {
    QImage frame(160, 144, QImage::Format_RGB32);
    PPU ppu(cpu.mmu->mem, &cpu.clock, &frame);

//    cpu.loadCartridge("D:/Roms/GB/Tetris.gb");
    cpu.mmu->loadCartridge("/Users/pscott/Documents/GB/Tetris.gb");

    auto cycleStart = std::chrono::system_clock::now();

    bool decoding = false;

    forever {
        ppu.prevClock = cpu.clock;
        cpu.step();
        ppu.currClock = cpu.clock;
        ppu.step();

        // send frame to widget
        if (cpu.mmu->mem[LY] > 143 && !emitted) {
            emitted = true;
            if (!ppu.lcdDisplayEnable()) {
                frame.fill(0xFFFFFF);
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