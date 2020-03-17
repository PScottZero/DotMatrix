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

    int i = 0;
    while (true) {
        cpu.step();
        dmw->ppu->render();
        if (i % 10 == 0) {
            dmw->ppu->render();
        }
        i++;
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

    thread cpu(initEmulator, &dmw);
    return a.exec();
    return 0;
}
