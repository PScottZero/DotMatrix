#ifndef DOTMATRIXWINDOW_H
#define DOTMATRIXWINDOW_H

#include <QMainWindow>
#include <ppu.h>

QT_BEGIN_NAMESPACE
namespace Ui { class DotMatrixWindow; }
QT_END_NAMESPACE

class DotMatrixWindow : public QMainWindow
{
    Q_OBJECT

public:
    DotMatrixWindow(QWidget *parent = nullptr);
    ~DotMatrixWindow();
    Ui::DotMatrixWindow *ui;
    PPU *ppu;
};
#endif // DOTMATRIXWINDOW_H
