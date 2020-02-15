#include "DotMatrix.h"
#include "GB_CPU.h"
#include "GB_PPU.h"
#include <QtWidgets/QApplication>
#include <thread>

void emuStart(DotMatrix *dm) {
	GB_CPU cpu;
	cpu.loadBootstrap();
	cpu.loadCartridge("C:/Users/8psco/Documents/Emulation/.roms/GB/Tetris.gb");
	dm->ui.ppu->setMemory(cpu.mem);
	int i = 0;
	while (cpu.PC <= 0x100) {
		cpu.run();
		if (i % 40 == 0) {
			dm->ui.ppu->render();
		}
		i++;
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DotMatrix dm;
	dm.show();
	
	thread thread_object(emuStart, &dm);
	return a.exec();
	return 0;
}
