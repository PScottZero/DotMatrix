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
    cpu.loadCartridge("D:/Roms/GB/cpu_instrs.gb");
//    cpu.loadCartridge("/Users/pscott/Documents/GB/Tetris.gb");

    auto cycleStart = chrono::system_clock::now();

    forever {
        cpu.step();
        ppu.step();

        // send frame to widget
        if (cpu.mem[LCDC_Y] > 144 && !emitted) {
            emitted = true;
            if (!ppu.lcdDisplayEnable()) {
                frame.fill(0xFFFFFF);
            }
            emit sendFrame(frame);
            auto nextCycle = cycleStart + chrono::milliseconds(16);
            this_thread::sleep_until(nextCycle);
            cycleStart = chrono::system_clock::now();
        } else if (cpu.mem[LCDC_Y] <= 144) {
            emitted = false;
        }
    }
}

void GBThread::processInput(Joypad button, bool pressed) {
    bool oldState = cpu.joypad[button];
    cpu.joypad[button] = pressed;
    if (oldState != pressed) {
        cpu.mem[IF] |= 0x8;
    }
}

#pragma clang diagnostic pop