#include "dmWindow.h"
#include <QApplication>
#include <QFileDialog>

DMWindow::DMWindow() : QMainWindow() {
    setWindowTitle(tr("Dot Matrix v0.1 alpha"));
    setWindowIcon(QIcon(":/img/dm_icon.ico"));
    setFixedSize(640, 602);

    setStyleSheet("QMainWindow {border-image: url(:/img/background.png) 0 0 0 0 stretch stretch}");

    display = new DisplayWidget(this);
    gbthread = new GBThread(this);

    connect(this, SIGNAL(sendInput(Joypad, bool)), gbthread, SLOT(processInput(Joypad, bool)));
    connect(gbthread, SIGNAL(sendFrame(QImage)), display, SLOT(updateDisplay(QImage)));

    setCentralWidget(display);
    createMenuBar();
}

void DMWindow::createMenuBar() {
    auto *menu = new QMenuBar();

    // file menu
    QMenu *fileMenu = menu->addMenu(tr("&File"));

    QAction *load = fileMenu->addAction(tr("&Load ROM"));
    load->setShortcut(QKeySequence("Ctrl+L"));
    connect(load, &QAction::triggered, this, &DMWindow::loadRom);

    fileMenu->addSeparator();

    QAction *quit = fileMenu->addAction(tr("&Quit"));
    quit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quit, &QAction::triggered, this, &QApplication::quit);

    setMenuBar(menu);
}

void DMWindow::loadRom() {
    gbthread->terminate();
    QString rom = QFileDialog::getOpenFileName(this, tr("Load Rom"), nullptr, tr("Game Boy Rom (*.gb)"));
    gbthread->setRom(rom.toStdString());
    gbthread->start();
}

void DMWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case KEYCODE_M:
            emit sendInput(START, true);
            break;
        case KEYCODE_N:
            emit sendInput(SELECT, true);
            break;
        case KEYCODE_W:
            emit sendInput(UP, true);
            break;
        case KEYCODE_S:
            emit sendInput(DOWN, true);
            break;
        case KEYCODE_A:
            emit sendInput(LEFT, true);
            break;
        case KEYCODE_D:
            emit sendInput(RIGHT, true);
            break;
        case KEYCODE_P:
            emit sendInput(BUTTON_A, true);
            break;
        case KEYCODE_O:
            emit sendInput(BUTTON_B, true);
            break;
    }
}

void DMWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case KEYCODE_M:
            emit sendInput(START, false);
            break;
        case KEYCODE_N:
            emit sendInput(SELECT, false);
            break;
        case KEYCODE_W:
            emit sendInput(UP, false);
            break;
        case KEYCODE_S:
            emit sendInput(DOWN, false);
            break;
        case KEYCODE_A:
            emit sendInput(LEFT, false);
            break;
        case KEYCODE_D:
            emit sendInput(RIGHT, false);
            break;
        case KEYCODE_P:
            emit sendInput(BUTTON_A, false);
            break;
        case KEYCODE_O:
            emit sendInput(BUTTON_B, false);
            break;
    }
}
