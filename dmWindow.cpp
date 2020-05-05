#include "dmWindow.h"

// ================================
// Constructor
// ================================
DMWindow::DMWindow() : QMainWindow() {
    setWindowTitle(tr("Dot Matrix v0.1 alpha"));
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

    QMenu *palette = optionsMenu->addMenu("Palette");
    auto *paletteSignal = new QSignalMapper(this);
    addPalette(palette, paletteSignal, "BGB", palBGB);
    addPalette(palette, paletteSignal, "Bicycle (By Braquen)", palBicycle);
    addPalette(palette, paletteSignal, "Chocolate (By GrafxKid)", palChocolate);
    addPalette(palette, paletteSignal, "Cobalt", palCobalt);
    addPalette(palette, paletteSignal, "Game Boy Original", palDMG);
    addPalette(palette, paletteSignal, "Game Boy Pocket (Default)", palGBP);
    addPalette(palette, paletteSignal, "Inverted", palInvert);
    addPalette(palette, paletteSignal, "Kirby (Super Game Boy)", palKirby);
    addPalette(palette, paletteSignal, "Platinum (By WildLeoKnight)", palPlatinum);
    addPalette(palette, paletteSignal, "Pokemon (Super Game Boy)", palPokemon);
    addPalette(palette, paletteSignal, "Wish GB (By Kerrie Lake)", palWish);

    connect(paletteSignal, SIGNAL(mapped(QObject*)), this, SLOT(setPalette(QObject*)));

    setMenuBar(menu);
}

// ================================
// Set Game Boy palette
// ================================
void DMWindow::setPalette(QObject* pal) const {
    gbthread->ppu->setPalette(qobject_cast<Palette*>(pal));
}

// ================================
// Add Game Boy palette to UI
// ================================
void DMWindow::addPalette(QMenu* palMenu, QSignalMapper *sigMap, const std::string& name, Palette* pal) {
    QAction *palette = palMenu->addAction(name.c_str());
    connect(palette, SIGNAL(triggered()), sigMap, SLOT(map()));
    sigMap->setMapping(palette, pal);
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
