#include "dmWindow.h"

// ================================
// Constructor
// ================================
DMWindow::DMWindow() : QMainWindow() {
    setWindowTitle(tr("Dot Matrix v0.5 βeta"));
    setWindowIcon(QIcon(":/img/dm_icon.ico"));

    setStyleSheet("QMainWindow {border-image: url(:/img/background.png) 0 0 0 0 stretch stretch}");

    display = new DisplayWidget(this);
    gbthread = new GBThread(this);

    connect(this, SIGNAL(sendInput(Joypad, bool)), gbthread, SLOT(processInput(Joypad, bool)));
    connect(gbthread, SIGNAL(sendFrame(QImage)), display, SLOT(updateDisplay(QImage)));
    connect(gbthread, SIGNAL(sendBankError(unsigned char)), this, SLOT(bankError(unsigned char)));

    setCentralWidget(display);
    createMenuBar();
    adjustSize();
    setFixedSize(size());
}

// ================================
// Create menu bar for UI
// ================================
void DMWindow::createMenuBar() {
    auto *menu = new QMenuBar();

    // file menu
    QMenu *fileMenu = menu->addMenu(tr("File"));

    QAction *load = fileMenu->addAction(tr("Load ROM"));
    load->setShortcut(QKeySequence("Ctrl+L"));
    connect(load, &QAction::triggered, this, &DMWindow::loadRom);

    fileMenu->addSeparator();

    QAction *quit = fileMenu->addAction(tr("Quit"));
    quit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quit, &QAction::triggered, this, &QApplication::quit);


    // option menu
    QMenu *optionsMenu = menu->addMenu(tr("Options"));

    // palette options
    QMenu *palette = optionsMenu->addMenu("Palette");
    auto *palGroup = new QActionGroup(this);
    palGroup->setExclusive(true);
    auto *paletteSignal = new QSignalMapper(this);
    addPalette(palette, paletteSignal, palGroup, "BGB", palBGB, false);
    addPalette(palette, paletteSignal, palGroup, "Bicycle (By Braquen)", palBicycle, false);
    addPalette(palette, paletteSignal, palGroup, "Chocolate (By GrafxKid)", palChocolate, false);
    addPalette(palette, paletteSignal, palGroup, "Cobalt", palCobalt, false);
    addPalette(palette, paletteSignal, palGroup, "Game Boy Original", palDMG, false);
    addPalette(palette, paletteSignal, palGroup, "Game Boy Pocket", palGBP, true);
    addPalette(palette, paletteSignal, palGroup, "Inverted", palInvert, false);
    addPalette(palette, paletteSignal, palGroup, "Kirby (Super Game Boy)", palKirby, false);
    addPalette(palette, paletteSignal, palGroup, "Platinum (By WildLeoKnight)", palPlatinum, false);
    addPalette(palette, paletteSignal, palGroup, "Pokemon (Super Game Boy)", palPokemon, false);
    addPalette(palette, paletteSignal, palGroup, "Virtual Boy", palVB, false);
    addPalette(palette, paletteSignal, palGroup, "Wish GB (By Kerrie Lake)", palWish, false);
    connect(paletteSignal, SIGNAL(mapped(QObject*)), this, SLOT(setPalette(QObject*)));

    // speed options
    QMenu *speed = optionsMenu->addMenu("Speed");
    auto *speedGroup = new QActionGroup(this);
    speedGroup->setExclusive(true);
    auto *speedSignal = new QSignalMapper(this);
    addSpeedOption(speed, speedSignal, speedGroup, "4x", 4, false);
    addSpeedOption(speed, speedSignal, speedGroup, "2x", 8, false);
    addSpeedOption(speed, speedSignal, speedGroup, "1x", 16, true);
    addSpeedOption(speed, speedSignal, speedGroup, "0.5x", 32, false);
    addSpeedOption(speed, speedSignal, speedGroup, "0.25x", 64, false);
    connect(speedSignal, SIGNAL(mapped(int)), this, SLOT(setSpeed(int)));


    setMenuBar(menu);
}

// ================================
// Set Game Boy palette
// ================================
void DMWindow::setPalette(QObject* pal) const {
    gbthread->ppu->setPalette(qobject_cast<Palette*>(pal));
}

// ================================
// Set emulation palette
// ================================
void DMWindow::setSpeed(int speed) const {
    gbthread->setSpeed(speed);
}

// ================================
// Add Game Boy palette to UI
// ================================
void DMWindow::addPalette(QMenu *palMenu, QSignalMapper *sigMap, QActionGroup *actionGroup,
        const std::string& name, Palette *pal, bool checked) {
    QAction *palette = actionGroup->addAction(name.c_str());
    palMenu->addAction(palette);
    sigMap->setMapping(palette, pal);
    palette->setCheckable(true);
    palette->setChecked(checked);
    connect(palette, SIGNAL(triggered()), sigMap, SLOT(map()));
}

// ================================
// Add speed option to UI
// ================================
void DMWindow::addSpeedOption(QMenu *speedMenu, QSignalMapper *sigMap, QActionGroup *actionGroup,
        const std::string& name, int speed, bool checked) {
    QAction *speedOption = actionGroup->addAction(name.c_str());
    speedMenu->addAction(speedOption);
    sigMap->setMapping(speedOption, speed);
    speedOption->setCheckable(true);
    speedOption->setChecked(checked);
    connect(speedOption, SIGNAL(triggered()), sigMap, SLOT(map()));
}

// ================================
// Open ROM from file manager
// ================================
void DMWindow::loadRom() {
    gbthread->terminate();
    gbthread->wait();
    gbthread->saveRAM();
    QString rom = QFileDialog::getOpenFileName(this, tr("Load Rom"), "/", tr("Game Boy Rom (*.gb)"));
    if (!rom.isEmpty()) {
        gbthread->setRom(rom.toStdString());
        gbthread->start();
    }
}

// ================================
// Display bank error dialog
// ================================
void DMWindow::bankError(unsigned char bankType) {
    QMessageBox bankError;
    std::string err = "Unsupported Bank Type " + std::to_string(bankType);
    bankError.setWindowIcon(QIcon(":/img/dm_icon.ico"));
    bankError.setIcon(QMessageBox::Critical);
    bankError.setWindowTitle("Bank Type Error");
    bankError.setText(err.c_str());
    bankError.exec();
}

// ================================
// Check for key press
// ================================
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

// ================================
// Check for key release
// ================================
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
