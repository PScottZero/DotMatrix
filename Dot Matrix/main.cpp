#include "DotMatrix.h"'
#include "GB_CPU.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DotMatrix w;
	w.show();
	return a.exec();
	return 0;
}
