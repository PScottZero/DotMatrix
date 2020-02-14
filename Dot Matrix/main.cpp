#include "DotMatrix.h"'
#include "GB_CPU.h"
#include "GB_PPU.h"
#include <QtWidgets/QApplication>
#include <thread>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DotMatrix w;
	w.show();
	
	GB_CPU cpu;
	GB_PPU ppu(cpu.mem, w.scene);
	cpu.loadBootstrap();
	cpu.loadCartridge("C:/Users/8psco/Documents/Emulation/.roms/GB/Tetris.gb");
	while (cpu.PC <= 0x100) {
		cpu.run();
		ppu.render();
	}
	return a.exec();
	return 0;
}
