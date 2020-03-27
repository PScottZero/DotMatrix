#include "gbthread.h"

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    emitted = false;
}

GBThread::~GBThread()
{
    wait();
}

void GBThread::run() {
    CPU cpu;
    QImage frame(160, 144, QImage::Format_RGB32);
    PPU ppu(cpu.mem, &cpu.clock, &frame);

    cpu.loadBootstrap();
    cpu.loadCartridge("D:/Roms/GB/Tetris.gb");

    forever {
        cpu.step();
        ppu.step();
        if (cpu.mem[LCDC_Y] > 144 && !emitted) {
            emitted = true;
            emit sendFrame(frame);
        } else if (cpu.mem[LCDC_Y] <= 144) {
            emitted = false;
        }
    }
}
