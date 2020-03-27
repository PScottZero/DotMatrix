#include "gbwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GBWidget w;
    w.show();
    return a.exec();
}
