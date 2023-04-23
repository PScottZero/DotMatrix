#include "keybindingswindow.h"

#include "ui_keybindingswindow.h"

KeyBindingsWindow::KeyBindingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::KeyBindingsWindow) {
  ui->setupUi(this);
}

KeyBindingsWindow::~KeyBindingsWindow() { delete ui; }
