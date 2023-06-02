#pragma once

#include <math.h>

#include <QActionGroup>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QSignalMapper>

#include "../emulator/cgb.h"
#include "./ui_mainwindow.h"
#include "keybindingswindow.h"
#include "palettes.h"
#include "vramviewer.h"

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
  Ui::MainWindow *ui;
  CGB cgb;

  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void loadROM();
  void setScreen(const QImage *image);
  void setPalette(Palette *palette);
  void setScale(float scale);
  void openKeyBindingsWindow();
  void openVramViewer();
  void toggleLogging(bool enableLog);

 protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

 private:
  KeyBindingsWindow kbWin;
  VramViewer vramViewer;

  QString getPaletteLabel(Palette *palette);
};
