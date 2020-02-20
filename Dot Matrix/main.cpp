#include "DotMatrix.h"
#include "GB_CPU.h"
#include "GB_PPU.h"
#include <QtWidgets/QApplication>
#include <thread>

void initDisplay(DotMatrix *dm, GB_CPU *cpu, mutex *rw) {
	dm->ui.ppu->setMemory(cpu->mem);
	dm->ui.ppu->setMutex(rw);
	chrono::milliseconds clock(10);
	while (true) {
		auto x = chrono::steady_clock::now() + clock;
		dm->ui.ppu->render();
		this_thread::sleep_until(x);
	}
}

void initEmulator(DotMatrix *dm) {
	GB_CPU cpu;
	cpu.loadBootstrap();
	cpu.loadCartridge("C:/Users/8psco/Documents/Emulation/.roms/GB/Tetris.gb");
	
	// create display thread
	thread display(initDisplay, dm, &cpu, &(cpu.rw));
	
	chrono::nanoseconds clock(200);
	while (true) {
		auto x = chrono::steady_clock::now() + clock;
		cpu.step();
		this_thread::sleep_until(x);
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DotMatrix dm;
	dm.show();
	
	thread cpu(initEmulator, &dm);
	return a.exec();
	return 0;
}
