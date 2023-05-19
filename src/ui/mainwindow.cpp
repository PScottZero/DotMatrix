#include "mainwindow.h"

#include "../emulator/log.h"
#include "../emulator/ppu.h"
#include "keybindingswindow.h"
#include "vramviewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cgb(),
      kbWin(&cgb.controls),
      vramViewer(&cgb) {
  ui->setupUi(this);

  // **************************************************
  // File Menu
  // **************************************************
  connect(ui->actionOpenROM, &QAction::triggered, this, &MainWindow::loadROM);
  connect(ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

  // **************************************************
  // Emulation Menu
  // **************************************************
  connect(ui->actionPause, &QAction::toggled, &cgb, &CGB::togglePause);
  connect(ui->actionReset, &QAction::triggered, &cgb, &CGB::restart);

  // device options
  auto deviceGroup = new QActionGroup(this);
  ui->actionGameBoy->setActionGroup(deviceGroup);
  ui->actionGameBoyColor->setActionGroup(deviceGroup);
  connect(ui->actionGameBoy, &QAction::triggered, &cgb,
          [this] { cgb.setDevice(false); });
  connect(ui->actionGameBoyColor, &QAction::triggered, &cgb,
          [this] { cgb.setDevice(true); });

  connect(ui->actionSkipDmgBootstrap, &QAction::toggled, &cgb,
          &CGB::toggleDmgBootstrap);

  // **************************************************
  // Display Menu
  // **************************************************

  // scale options
  auto scaleGroup = new QActionGroup(this);
  scaleGroup->setExclusive(true);
  ui->action0_5x->setActionGroup(scaleGroup);
  ui->action1x->setActionGroup(scaleGroup);
  ui->action1_5x->setActionGroup(scaleGroup);
  ui->action2x->setActionGroup(scaleGroup);
  connect(ui->action0_5x, &QAction::triggered, this, [this] { setScale(0.5); });
  connect(ui->action1x, &QAction::triggered, this, [this] { setScale(1.0); });
  connect(ui->action1_5x, &QAction::triggered, this, [this] { setScale(1.5); });
  connect(ui->action2x, &QAction::triggered, this, [this] { setScale(2.0); });

  // palette options
  auto paletteGroup = new QActionGroup(this);
  paletteGroup->setExclusive(true);

  // add non-sgb palettes to palettes menu
  for (auto palette : Palettes::allPalettes) {
    QAction *action = new QAction();
    action->setCheckable(true);
    if (palette->name == "Game Boy Pocket") action->setChecked(true);
    action->setText(getPaletteLabel(palette));
    action->setActionGroup(paletteGroup);
    ui->menuPalette->addAction(action);
    connect(action, &QAction::triggered, this,
            [this, palette] { setPalette(palette); });
    connect(action, &QAction::hovered, this,
            [this, palette] { cgb.previewPalette(palette); });
  }

  // add sgb palettes to sgb palettes submenu
  for (auto sgbPalette : Palettes::sgbPalettes) {
    QAction *action = new QAction();
    action->setCheckable(true);
    action->setText(sgbPalette->name);
    action->setActionGroup(paletteGroup);
    ui->menuSGB->addAction(action);
    connect(action, &QAction::triggered, this,
            [this, sgbPalette] { setPalette(sgbPalette); });
    connect(action, &QAction::hovered, this,
            [this, sgbPalette] { cgb.previewPalette(sgbPalette); });
  }

  // reset palette preview
  connect(ui->menuPalette, &QMenu::aboutToHide, &cgb,
          &CGB::resetPreviewPalette);
  connect(ui->actionShowBackground, &QAction::toggled, &cgb.ppu,
          &PPU::toggleBackground);
  connect(ui->actionShowWindow, &QAction::toggled, &cgb.ppu,
          &PPU::toggleWindow);
  connect(ui->actionShowSprites, &QAction::toggled, &cgb.ppu,
          &PPU::toggleSprites);

  // **************************************************
  // Controls Menu
  // **************************************************
  connect(ui->actionKeyBindings, &QAction::triggered, this,
          &MainWindow::openKeyBindingsWindow);

  // **************************************************
  // Debug Menu
  // **************************************************
  connect(ui->actionVramViewer, &QAction::triggered, this,
          &MainWindow::openVramViewer);
  connect(ui->actionEnableLogging, &QAction::toggled, this,
          &MainWindow::toggleLogging);

  // cgb sends rendered screen to ui
  connect(&cgb, &CGB::sendScreen, this, &MainWindow::setScreen);

  // finalize setup
  cgb.actionPause = ui->actionPause;
  setScale(1);
}

MainWindow::~MainWindow() { delete ui; }

// get palette label, display authoriship if
// palette has a specified author
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
  QString romPath = QFileDialog::getOpenFileName(
      this, tr("Open File"), cgb.romPath, tr("Game Boy ROMs (*.gb *.gbc)"));
  cgb.pause = false;
  if (romPath != "") {
    setWindowTitle(romPath.split("/").last());
    cgb.romPath = romPath;
    cgb.reset();
    bool romSupported = cgb.loadRom(romPath);
    if (romSupported) cgb.start(QThread::HighestPriority);
  }
}

// **************************************************
// **************************************************
// QT Slots
// **************************************************
// **************************************************

// set screen to the given image
void MainWindow::setScreen(const QImage *image) {
  // no anti-aliasing when resizing image
  auto pixmap = QPixmap::fromImage(*image).scaled(
      ui->screen->width(), ui->screen->height(), Qt::KeepAspectRatio,
      Qt::FastTransformation);
  ui->screen->setPixmap(pixmap);
}

// set screen scale
void MainWindow::setScale(float scale) {
  int width = WINDOW_BASE_WIDTH * scale;
  int height = WINDOW_BASE_HEIGHT * scale;
#ifdef Q_OS_MAC
  this->setFixedSize(width, height);
#else
  this->setFixedSize(width, height + ui->menubar->height());
#endif
  ui->screen->setFixedSize(width, height);
}

// set dmg palette
void MainWindow::setPalette(Palette *palette) {
  cgb.ppu.palette = (Palette *)palette;
  cgb.tempPalette = nullptr;
  cgb.renderInPauseMode();
}

// open the key bindings window
void MainWindow::openKeyBindingsWindow() { kbWin.show(); }

// open vram viewer
void MainWindow::openVramViewer() { vramViewer.show(); }

// toggle logging
void MainWindow::toggleLogging(bool enableLog) { Log::enable = enableLog; }

// game boy button press event
void MainWindow::keyPressEvent(QKeyEvent *event) {
  cgb.controls.press(event->key());
}

// game boy button release event
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (!event->isAutoRepeat()) {
    cgb.controls.release(event->key());
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  kbWin.close();
  vramViewer.close();
}
