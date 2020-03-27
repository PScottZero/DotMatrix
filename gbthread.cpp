#include "gbthread.h"

GBThread::GBThread(QObject *parent) : QThread(parent)
{
    emitted = false;
}

GBThread::~GBThread()
{

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
            QImage frame1(160, 144, QImage::Format_RGB32);
        } else if (cpu.mem[LCDC_Y] <= 144) {
            emitted = false;
        }
        this_thread::sleep_for(chrono::nanoseconds(100));
    }
}
