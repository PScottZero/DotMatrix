#include "mainwindow.h"

#include <QPixmap>
#include <QSizePolicy>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowIcon(QIcon(":/img/dm_icon.ico"));

  ui->screen->setPixmap(QPixmap(":/img/background.png"));
  ui->screen->setScaledContents(true);
  ui->screen->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  this->setFixedSize(480, 459);
}

MainWindow::~MainWindow() { delete ui; }
