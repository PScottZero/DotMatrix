#include "mainwindow.h"

#include <QDir>

#include "../emulator/bootstrap.h"
#include "../emulator/ppu.h"
#include "keybindingswindow.h"
#include "memoryview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cgb(),
      currPath(QDir::currentPath()) {
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
  addToActionGroup(paletteActionGroup, ui->actionBGB, paletteSigMap,
                   &Palettes::palBGB);
  addToActionGroup(paletteActionGroup, ui->actionBicycle, paletteSigMap,
                   &Palettes::palBicycle);
  addToActionGroup(paletteActionGroup, ui->actionChocolate, paletteSigMap,
                   &Palettes::palChocolate);
  addToActionGroup(paletteActionGroup, ui->actionCobalt, paletteSigMap,
                   &Palettes::palCobalt);
  addToActionGroup(paletteActionGroup, ui->actionGameBoy, paletteSigMap,
                   &Palettes::palGB);
  addToActionGroup(paletteActionGroup, ui->actionGameBoyPocket, paletteSigMap,
                   &Palettes::palGBP);
  addToActionGroup(paletteActionGroup, ui->actionInverted, paletteSigMap,
                   &Palettes::palInverted);
  addToActionGroup(paletteActionGroup, ui->actionKirby, paletteSigMap,
                   &Palettes::palKirby);
  addToActionGroup(paletteActionGroup, ui->actionPlatinum, paletteSigMap,
                   &Palettes::palPlatinum);
  addToActionGroup(paletteActionGroup, ui->actionPokemon, paletteSigMap,
                   &Palettes::palPokemon);
  addToActionGroup(paletteActionGroup, ui->actionVirtualBoy, paletteSigMap,
                   &Palettes::palVB);
  addToActionGroup(paletteActionGroup, ui->actionWishGB, paletteSigMap,
                   &Palettes::palWishGB);
  connect(paletteSigMap, &QSignalMapper::mappedObject, this,
          &MainWindow::setPalette);

  connect(ui->actionKeyBindings, &QAction::triggered, this,
          &MainWindow::openKeyBindingsWindow);

  connect(ui->actionShowBootScreen, &QAction::toggled, this,
          &MainWindow::toggleBootScreen);

  // **************************************************
  // **************************************************
  // Debug Menu
  // **************************************************
  // **************************************************
  connect(ui->actionLoadSaveState, &QAction::triggered, this,
          &MainWindow::loadSaveState);
  connect(ui->actionMemoryView, &QAction::triggered, this,
          &MainWindow::openMemoryView);

  connect(&cgb, &CGB::sendScreen, this, &MainWindow::setScreen);

  // set main window size
  setScale(1);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::loadROM() {
  // select rom to run
  QString romName = QFileDialog::getOpenFileName(
      this, tr("Open File"), currPath, tr("Game Boy ROMs (*.gb *.gbc)"));
  if (romName != "") {
    currPath = romName;
    cgb.reset();
    bool romSupported = cgb.loadROM(romName);
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

void MainWindow::loadSaveState() {}

void MainWindow::openMemoryView() {
  MemoryView memView{};
  memView.exec();
}

void MainWindow::toggleBootScreen(bool showBootScreen) {
  Bootstrap::skip = !showBootScreen;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  Controls::press(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (!event->isAutoRepeat()) {
    Controls::release(event->key());
  }
}
