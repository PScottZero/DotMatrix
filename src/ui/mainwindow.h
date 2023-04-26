#pragma once

#include <math.h>

#include <QActionGroup>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QSignalMapper>

#include "../emulator/cgb.h"
#include "./ui_mainwindow.h"

#define WINDOW_BASE_WIDTH 640
#define WINDOW_BASE_HEIGHT 576

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void loadROM();
  void setScreen(QImage *image);
  void setPalette(QObject *palette);
  void setScale(int scale);
  void setSpeed(int speed);
  void openKeyBindingsWindow();
  void loadSaveState();
  void runBootstrap();
  void openMemoryView();

 protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
  Palette palBGB, palBicycle, palChocolate, palCobalt, palGB, palGBP,
      palInverted, palKirby, palPlatinum, palPokemon, palVB, palWishGB;

  Ui::MainWindow *ui;
  CGB cgb;

  void addToActionGroup(QActionGroup *actionGroup, QAction *action,
                        QSignalMapper *sigMap, int mapValue);
  void addToActionGroup(QActionGroup *actionGroup, QAction *action,
                        QSignalMapper *sigMap, Palette *mapValue);
};
