#include "mainwindow.h"

#include "../emulator/bootstrap.h"
#include "../emulator/log.h"
#include "../emulator/ppu.h"
#include "keybindingswindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cgb() {
  ui->setupUi(this);

  cgb.actionPause = ui->actionPause;

  // **************************************************
  // **************************************************
  // File Menu
  // **************************************************
  // **************************************************
  connect(ui->actionOpenROM, &QAction::triggered, this, &MainWindow::loadROM);
  connect(ui->actionPause, &QAction::toggled, this, &MainWindow::pause);
  connect(ui->actionReset, &QAction::triggered, this, &MainWindow::reset);
  connect(ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

  // **************************************************
  // **************************************************
  // Settings Menu
  // **************************************************
  // **************************************************

  // **************************************************
  // Scale Options
  // **************************************************
  auto scaleActionGroup = new QActionGroup(this);
  auto scaleSigMap = new QSignalMapper(this);
  scaleActionGroup->setExclusive(true);
  addToActionGroup(scaleActionGroup, ui->actionScale0_5x, scaleSigMap, 0);
  addToActionGroup(scaleActionGroup, ui->actionScale1x, scaleSigMap, 1);
  addToActionGroup(scaleActionGroup, ui->actionScale1_5x, scaleSigMap, 2);
  addToActionGroup(scaleActionGroup, ui->actionScale2x, scaleSigMap, 3);
  connect(scaleSigMap, &QSignalMapper::mappedInt, this, &MainWindow::setScale);

  // **************************************************
  // Speed Options
  // **************************************************
  auto speedActionGroup = new QActionGroup(this);
  auto speedSigMap = new QSignalMapper(this);
  speedActionGroup->setExclusive(true);
  addToActionGroup(speedActionGroup, ui->actionSpeed0_25x, speedSigMap, 0);
  addToActionGroup(speedActionGroup, ui->actionSpeed0_5x, speedSigMap, 1);
  addToActionGroup(speedActionGroup, ui->actionSpeed1x, speedSigMap, 2);
  addToActionGroup(speedActionGroup, ui->actionSpeed2x, speedSigMap, 3);
  addToActionGroup(speedActionGroup, ui->actionSpeed4x, speedSigMap, 4);
  connect(speedSigMap, &QSignalMapper::mappedInt, this, &MainWindow::setSpeed);

  // **************************************************
  // Palette Options
  // **************************************************
  auto paletteActionGroup = new QActionGroup(this);
  auto paletteSigMap = new QSignalMapper(this);
  paletteActionGroup->setExclusive(true);
  for (auto pal : Palettes::allPalettes) {
    QAction *action = new QAction();
    action->setCheckable(true);
    if (pal->name == "Game Boy Pocket") action->setChecked(true);
    QString label = pal->name;
    if (pal->creator != "Me") {
      if (pal->creator == "SGB") {
        label += " (Super Game Boy)";
      } else {
        label += " (By " + pal->creator + ")";
      }
    }
    action->setText(label);
    ui->menuPalette->addAction(action);
    addToActionGroup(paletteActionGroup, action, paletteSigMap, pal);
  }
  connect(paletteSigMap, &QSignalMapper::mappedObject, this,
          &MainWindow::setPalette);

  // **************************************************
  // Other Options
  // **************************************************
  connect(ui->actionKeyBindings, &QAction::triggered, this,
          &MainWindow::openKeyBindingsWindow);
  connect(ui->actionShowBootScreen, &QAction::toggled, this,
          &MainWindow::toggleBootScreen);

  // **************************************************
  // **************************************************
  // Debug Menu
  // **************************************************
  // **************************************************
  connect(ui->actionShowBackground, &QAction::toggled, this,
          &MainWindow::toggleBackground);
  connect(ui->actionShowWindow, &QAction::toggled, this,
          &MainWindow::toggleWindow);
  connect(ui->actionShowSprites, &QAction::toggled, this,
          &MainWindow::toggleSprites);
  connect(ui->actionEnableLogging, &QAction::toggled, this,
          &MainWindow::toggleLogging);

  connect(&cgb, &CGB::sendScreen, this, &MainWindow::setScreen);

  // set main window size
  setScale(1);
}

MainWindow::~MainWindow() { delete ui; }

// select rom to run
void MainWindow::loadROM() {
  cgb.pause = true;
  QString romName = QFileDialog::getOpenFileName(
      this, tr("Open File"), CGB::romPath, tr("Game Boy ROMs (*.gb *.gbc)"));
  cgb.pause = false;
  if (romName != "") {
    CGB::romPath = romName;
    cgb.reset();
    bool romSupported = cgb.loadRom(romName);
    if (romSupported) cgb.start();
  }
}

void MainWindow::pause(bool shouldPause) { cgb.pause = shouldPause; }

void MainWindow::reset() {
  cgb.reset();
  cgb.start();
}

void MainWindow::setScreen(QImage image) {
  // no anti-aliasing when resizing image
  auto pixmap = QPixmap::fromImage(image).scaled(
      ui->screen->width(), ui->screen->height(), Qt::KeepAspectRatio,
      Qt::FastTransformation);
  ui->screen->setPixmap(pixmap);
}

void MainWindow::setScale(int scale) {
  float scaleFloat = 0.5 + scale * 0.5;
  int width = WINDOW_BASE_WIDTH * scaleFloat;
  int height = WINDOW_BASE_HEIGHT * scaleFloat;
#ifdef Q_OS_MAC
  this->setFixedSize(width, height);
#else
  this->setFixedSize(width, height + ui->menubar->height());
#endif
  ui->screen->setFixedSize(width, height);
}

void MainWindow::setSpeed(int speed) { cgb.speedMult = pow(2, speed - 2); }

void MainWindow::addToActionGroup(QActionGroup *actionGroup, QAction *action,
                                  QSignalMapper *sigMap, int mapVal) {
  action->setActionGroup(actionGroup);
  sigMap->setMapping(action, mapVal);
  connect(action, SIGNAL(triggered()), sigMap, SLOT(map()));
}

void MainWindow::addToActionGroup(QActionGroup *actionGroup, QAction *action,
                                  QSignalMapper *sigMap, Palette *mapVal) {
  action->setActionGroup(actionGroup);
  sigMap->setMapping(action, mapVal);
  connect(action, SIGNAL(triggered()), sigMap, SLOT(map()));
}

void MainWindow::setPalette(QObject *palette) {
  PPU::palette = (Palette *)palette;
}

void MainWindow::openKeyBindingsWindow() {
  KeyBindingsWindow kbWin{};
  kbWin.exec();
}

void MainWindow::toggleBootScreen(bool showBootScreen) {
  Bootstrap::skip = !showBootScreen;
}

void MainWindow::toggleBackground(bool showBackground) {
  PPU::showBackground = showBackground;
}

void MainWindow::toggleWindow(bool showWindow) { PPU::showWindow = showWindow; }

void MainWindow::toggleSprites(bool showSprites) {
  PPU::showSprites = showSprites;
}

void MainWindow::toggleLogging(bool enableLog) { Log::enable = enableLog; }

void MainWindow::keyPressEvent(QKeyEvent *event) {
  Controls::press(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (!event->isAutoRepeat()) {
    Controls::release(event->key());
  }
}
