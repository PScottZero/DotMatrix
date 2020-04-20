#ifndef DOTMATRIX_DMWINDOW_H
#define DOTMATRIX_DMWINDOW_H

#include "displayWidget.h"
#include "gbThread.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>

class DMWindow : public QMainWindow {
Q_OBJECT

public:

    DisplayWidget *display;
    GBThread *gbthread;

    DMWindow();
    void createMenuBar();
    void loadRom();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void sendInput(Joypad button, bool pressed);
};


#endif //DOTMATRIX_DMWINDOW_H
