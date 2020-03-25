#include "dotmatrixwindow.h"
#include "cpu.h"
#include "ppu.h"
#include <thread>

#include <QApplication>

// ================================
// Initialize emulation
// ================================s
void initEmulator(DotMatrixWindow *dmw) {
    CPU cpu;
    cpu.loadBootstrap();
    cpu.loadCartridge("D:/Roms/GB/Tetris.gb");

    dmw->ppu->setMemory(cpu.mem);
    dmw->ppu->setCPUClock(&cpu.clock);

    while (true) {
        cpu.step();
        dmw->ppu->step();
    }
}

// ================================
// Main function
// ================================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DotMatrixWindow dmw;
    dmw.show();
    initEmulator(&dmw);
    return a.exec();
    return 0;
}
