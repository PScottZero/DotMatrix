#include "dotmatrixwindow.h"
#include "ui_dotmatrixwindow.h"

DotMatrixWindow::DotMatrixWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DotMatrixWindow)
{
    ui->setupUi(this);
    ppu = ui->ppu;
}

DotMatrixWindow::~DotMatrixWindow()
{
    delete ui;
}

