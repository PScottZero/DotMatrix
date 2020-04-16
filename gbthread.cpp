#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbthread.h"
#include <thread>

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    emitted = false;
}

GBThread::~GBThread() = default;

void GBThread::run() {
    QImage frame(160, 144, QImage::Format_RGB32);
    PPU ppu(cpu.mem, &cpu.clock, &frame);

    cpu.loadBootstrap();
    cpu.loadCartridge("D:/Roms/GB/Dr. Mario.gb");
//    cpu.loadCartridge("/Users/pscott/Documents/GB/Tetris.gb");

    auto cycleStart = chrono::system_clock::now();

    forever {
        ppu.prevClock = cpu.clock;
        cpu.step();
        ppu.currClock = cpu.clock;
        ppu.step();

        // send frame to widget
        if (cpu.mem[LY] > 143 && !emitted) {
            emitted = true;
            if (!ppu.lcdDisplayEnable()) {
                frame.fill(0xFFFFFF);
            }
            emit sendFrame(frame);
            auto nextCycle = cycleStart + chrono::milliseconds(16);
            this_thread::sleep_until(nextCycle);
            cycleStart = chrono::system_clock::now();
        } else if (cpu.mem[LY] <= 143) {
            emitted = false;
        }
    }
}

void GBThread::processInput(Joypad button, bool pressed) {
    bool oldState = cpu.joypad[button];
    cpu.joypad[button] = pressed;
    if (!oldState && cpu.joypad[button]) {
        cpu.mem[IF] |= 0x10;
    }
}

#pragma clang diagnostic pop