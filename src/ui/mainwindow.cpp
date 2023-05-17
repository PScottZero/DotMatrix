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
  addToActionGroup(scaleActionGroup, ui->action0_5x, scaleSigMap, 0);
  addToActionGroup(scaleActionGroup, ui->action1x, scaleSigMap, 1);
  addToActionGroup(scaleActionGroup, ui->action1_5x, scaleSigMap, 2);
  addToActionGroup(scaleActionGroup, ui->action2x, scaleSigMap, 3);
  connect(scaleSigMap, &QSignalMapper::mappedInt, this, &MainWindow::setScale);

  // **************************************************
  // Palette Options
  // **************************************************
  auto paletteActionGroup = new QActionGroup(this);
  auto paletteSigMap = new QSignalMapper(this);
  paletteActionGroup->setExclusive(true);

  // add non-sgb palettes to palettes menu
  for (auto palette : Palettes::allPalettes) {
    QAction *action = new QAction();
    action->setCheckable(true);
    if (palette->name == "Game Boy Pocket") action->setChecked(true);
    action->setText(getPaletteLabel(palette));
    ui->menuPalette->addAction(action);
    addToActionGroup(paletteActionGroup, action, paletteSigMap, palette);
    connect(action, &QAction::hovered, this,
            [this, palette] { cgb.previewPalette(palette); });
  }

  // add sgb palettes to sgb palettes submenu
  for (auto sgbPalette : Palettes::sgbPalettes) {
    QAction *action = new QAction();
    action->setCheckable(true);
    action->setText(sgbPalette->name);
    ui->menuSGB->addAction(action);
    addToActionGroup(paletteActionGroup, action, paletteSigMap, sgbPalette);
    connect(action, &QAction::hovered, this,
            [this, sgbPalette] { cgb.previewPalette(sgbPalette); });
  }

  // connect palette signal map
  connect(paletteSigMap, &QSignalMapper::mappedObject, this,
          &MainWindow::setPalette);
  connect(ui->menuPalette, &QMenu::aboutToHide, &cgb,
          &CGB::resetPreviewPalette);

  // **************************************************
  // Other Options
  // **************************************************
  connect(ui->actionKeyBindings, &QAction::triggered, this,
          &MainWindow::openKeyBindingsWindow);
  connect(ui->actionShowBootScreen, &QAction::toggled, this,
          &MainWindow::toggleBootScreen);
  connect(ui->actionDmgMode, &QAction::toggled, this,
          &MainWindow::toggleDmgMode);

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

QString MainWindow::getPaletteLabel(Palette *palette) {
  QString label = palette->name;
  if (palette->creator != "") {
    label += " (By " + palette->creator + ")";
  }
  return label;
}

// select rom to run
void MainWindow::loadROM() {
  cgb.pause = true;
  QString romName = QFileDialog::getOpenFileName(
      this, tr("Open File"), cgb.romPath, tr("Game Boy ROMs (*.gb *.gbc)"));
  cgb.pause = false;
  if (romName != "") {
    cgb.romPath = romName;
    cgb.reset();
    bool romSupported = cgb.loadRom(romName);
    if (romSupported) cgb.start();
  }
}

void MainWindow::pause(bool shouldPause) { cgb.pause = shouldPause; }

void MainWindow::reset() {
  cgb.reset(false);
  cgb.start();
}

void MainWindow::setScreen(const QImage *image) {
  // no anti-aliasing when resizing image
  auto pixmap = QPixmap::fromImage(*image).scaled(
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

void MainWindow::setPalette(QObject *palette) {
  cgb.ppu.palette = (Palette *)palette;
  cgb.tempPalette = nullptr;
  cgb.renderInPauseMode();
}

void MainWindow::openKeyBindingsWindow() {
  KeyBindingsWindow kbWin(&cgb.controls);
  kbWin.exec();
}

void MainWindow::toggleBootScreen(bool showBootScreen) {
  cgb.bootstrap.skip = !showBootScreen;
}

void MainWindow::toggleBackground(bool showBackground) {
  cgb.ppu.showBackground = showBackground;
}

void MainWindow::toggleWindow(bool showWindow) {
  cgb.ppu.showWindow = showWindow;
}

void MainWindow::toggleSprites(bool showSprites) {
  cgb.ppu.showSprites = showSprites;
}

void MainWindow::toggleLogging(bool enableLog) { Log::enable = enableLog; }

void MainWindow::toggleDmgMode(bool dmgMode) {
  cgb.dmgMode = dmgMode;
  cgb.reset();
  cgb.start();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  cgb.controls.press(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (!event->isAutoRepeat()) {
    cgb.controls.release(event->key());
  }
}
