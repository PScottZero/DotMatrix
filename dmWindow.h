#ifndef DOTMATRIX_DMWINDOW_H
#define DOTMATRIX_DMWINDOW_H

#include "displayWidget.h"
#include "gbThread.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QtWidgets/QMessageBox>

class DMWindow : public QMainWindow {
Q_OBJECT

public:
    DisplayWidget *display;
    GBThread *gbthread;

    Palette *palCobalt = new Palette(0xC5DFFA, 0x5C88B8, 0x1A4778, 0x00234A);
    Palette *palGBP = new Palette(0xFFFFFF, 0xAAAAAA, 0x555555, 0x000000);
    Palette *palDMG = new Palette(0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F);
    Palette *palBGB = new Palette(0xE0F8D0, 0x88C070, 0x346856, 0x081820);
    Palette *palPlatinum = new Palette(0xE0F0E8, 0xA8C0B0, 0x4F7868, 0x183030);
    Palette *palWish = new Palette(0x612F4C, 0x7450E9, 0x5F8FCF, 0x8BE5FF);
    Palette *palBicycle = new Palette(0xF0F0F0, 0x8F9BF6, 0xAB4645, 0x161616);
    Palette *palChocolate = new Palette(0xFFE4C2, 0xDCA456, 0xA9604C, 0x422936);
    Palette *palKirby = new Palette(0xF7BEF7, 0xE78686, 0x7732E7, 0x2D2B96);
    Palette *palInvert = new Palette(0x000000, 0x545454, 0xA9A9A9, 0xFFFFFF);
    Palette *palPokemon = new Palette(0xFFEFFF, 0xF7B58C, 0x84739C, 0x181010);
    Palette *palVB = new Palette(0x000000, 0x550000, 0xAA0000, 0xFF0000);

    DMWindow();
    void createMenuBar();
    void loadRom();
    void addPalette(QMenu *palMenu, QSignalMapper *sigMap, QActionGroup *actionGroup,
            const std::string& name, Palette *, bool checked);
    void addSpeedOption(QMenu *speedMenu, QSignalMapper *sigMap, QActionGroup *actionGroup,
            const std::string& name, int speed, bool checked);

public slots:
    void setPalette(QObject *pal) const;
    void setSpeed(int speed) const;
    void bankError(unsigned char);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void sendInput(Joypad button, bool pressed);
};

#endif //DOTMATRIX_DMWINDOW_H
