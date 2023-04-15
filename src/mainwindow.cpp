#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      palBGB(0xE0F8D0, 0x88C070, 0x346856, 0x081820),
      palBicycle(0xF0F0F0, 0x8F9BF6, 0xAB4645, 0x161616),
      palChocolate(0xFFE4C2, 0xDCA456, 0xA9604C, 0x422936),
      palCobalt(0xC5DFFA, 0x5C88B8, 0x1A4778, 0x00234A),
      palGB(0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F),
      palGBP(0xFFFFFF, 0xAAAAAA, 0x555555, 0x000000),
      palInverted(0x000000, 0x545454, 0xA9A9A9, 0xFFFFFF),
      palKirby(0xF7BEF7, 0xE78686, 0x7732E7, 0x2D2B96),
      palPlatinum(0xE0F0E8, 0xA8C0B0, 0x4F7868, 0x183030),
      palPokemon(0xFFEFFF, 0xF7B58C, 0x84739C, 0x181010),
      palVB(0x000000, 0x550000, 0xAA0000, 0xFF0000),
      palWishGB(0x612F4C, 0x7450E9, 0x5F8FCF, 0x8BE5FF),
      cgb(&palGB),
      ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // **************************************************
  // **************************************************
  // File Menu
  // **************************************************
  // **************************************************
  connect(ui->actionOpenROM, &QAction::triggered, this, &MainWindow::loadROM);
  connect(ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

  // **************************************************
  // **************************************************
  // Settings Menu
  // **************************************************
  // **************************************************

  // ==================================================
  // Scale Options
  // ==================================================
  auto scaleActionGroup = new QActionGroup(this);
  auto scaleSigMap = new QSignalMapper(this);
  scaleActionGroup->setExclusive(true);
  addToActionGroup(scaleActionGroup, ui->actionScale05x, scaleSigMap, 0);
  addToActionGroup(scaleActionGroup, ui->actionScale1x, scaleSigMap, 1);
  addToActionGroup(scaleActionGroup, ui->actionScale2x, scaleSigMap, 2);
  addToActionGroup(scaleActionGroup, ui->actionScale3x, scaleSigMap, 3);
  connect(scaleSigMap, SIGNAL(mapped(int)), this, SLOT(setScale(int)));

  // ==================================================
  // Speed Options
  // ==================================================
  auto speedActionGroup = new QActionGroup(this);
  auto speedSigMap = new QSignalMapper(this);
  speedActionGroup->setExclusive(true);
  addToActionGroup(speedActionGroup, ui->actionSpeed025x, speedSigMap, 0);
  addToActionGroup(speedActionGroup, ui->actionSpeed05x, speedSigMap, 1);
  addToActionGroup(speedActionGroup, ui->actionSpeed1x, speedSigMap, 2);
  addToActionGroup(speedActionGroup, ui->actionSpeed2x, speedSigMap, 3);
  addToActionGroup(speedActionGroup, ui->actionSpeed4x, speedSigMap, 4);
  addToActionGroup(speedActionGroup, ui->actionSpeed8x, speedSigMap, 5);
  connect(speedSigMap, SIGNAL(mapped(int)), this, SLOT(setSpeed(int)));

  // ==================================================
  // Palette Options
  // ==================================================
  auto paletteActionGroup = new QActionGroup(this);
  auto paletteSigMap = new QSignalMapper(this);
  paletteActionGroup->setExclusive(true);
  addToActionGroup(paletteActionGroup, ui->actionBGB, paletteSigMap, &palBGB);
  addToActionGroup(paletteActionGroup, ui->actionBicycle, paletteSigMap,
                   &palBicycle);
  addToActionGroup(paletteActionGroup, ui->actionChocolate, paletteSigMap,
                   &palChocolate);
  addToActionGroup(paletteActionGroup, ui->actionCobalt, paletteSigMap,
                   &palCobalt);
  addToActionGroup(paletteActionGroup, ui->actionGameBoy, paletteSigMap,
                   &palGB);
  addToActionGroup(paletteActionGroup, ui->actionGameBoyPocket, paletteSigMap,
                   &palGBP);
  addToActionGroup(paletteActionGroup, ui->actionInverted, paletteSigMap,
                   &palInverted);
  addToActionGroup(paletteActionGroup, ui->actionKirby, paletteSigMap,
                   &palKirby);
  addToActionGroup(paletteActionGroup, ui->actionPlatinum, paletteSigMap,
                   &palPlatinum);
  addToActionGroup(paletteActionGroup, ui->actionPokemon, paletteSigMap,
                   &palPokemon);
  addToActionGroup(paletteActionGroup, ui->actionVirtualBoy, paletteSigMap,
                   &palVB);
  addToActionGroup(paletteActionGroup, ui->actionWishGB, paletteSigMap,
                   &palWishGB);
  connect(paletteSigMap, SIGNAL(mapped(QObject *)), this,
          SLOT(setPalette(QObject *)));

  connect(&cgb.ppu, SIGNAL(sendScreen(QImage *)), this, SLOT(setScreen(QImage *)));

  // set main window size
  setScale(1);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::loadROM() {
  QString romName = QFileDialog::getOpenFileName(this, tr("Open File"), ".",
                                                 tr("Game Boy ROMs (*.gb)"));
  cgb.loadROM(romName);
  cgb.run();
}

void MainWindow::setScreen(QImage *image) {
  ui->screen->setPixmap(QPixmap::fromImage(*image));
}

void MainWindow::setScale(int scale) {
  float scaleFloat = scale == 0 ? 0.5 : scale;
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
  cgb.palette = (Palette *)palette;
}
