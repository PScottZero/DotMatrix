#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include "gbthread.h"

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    emitted = false;
}

GBThread::~GBThread() = default;

void GBThread::run() {
    CPU cpu;
    QImage frame(160, 144, QImage::Format_RGB32);
    PPU ppu(cpu.mem, &cpu.clock, &frame);

    cpu.loadBootstrap();
    cpu.loadCartridge("D:/Roms/GB/Tetris.gb");
//    cpu.loadCartridge("/Users/pscott/Documents/GB/Tetris.gb");

    auto cycleStart = chrono::system_clock::now();

    forever {
        cpu.step();
        ppu.step();

        if (cpu.mem[LCDC_Y] > 144 && !emitted) {
            emitted = true;
            emit sendFrame(frame);
            auto nextCycle = cycleStart + chrono::milliseconds(16);
            this_thread::sleep_until(nextCycle);
            cycleStart = chrono::system_clock::now();
        } else if (cpu.mem[LCDC_Y] <= 144) {
            emitted = false;
        }
    }
}

#pragma clang diagnostic pop