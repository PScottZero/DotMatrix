#include "dmWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DMWindow dmw;
    dmw.show();
    return QApplication::exec();
}
