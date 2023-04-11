#include <QApplication>

#include "cgb.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  // create qapplication window
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  // create emulator
  auto cgb = CGB();
  cgb.run();

  return a.exec();
}
