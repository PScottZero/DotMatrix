#include "keybindingswindow.h"

#include <QKeySequence>

#include "ui_keybindingswindow.h"

KeyBindingsWindow::KeyBindingsWindow(Controls &controls, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::KeyBindingsWindow),
      controls(controls),
      selectedButton(UP),
      acceptKeyPress(false),
      buttonKeyLabels(),
      setKeyButtons() {
  ui->setupUi(this);

  buttonKeyLabels = {{UP, ui->upKeyLabel},       {DOWN, ui->downKeyLabel},
                     {LEFT, ui->leftKeyLabel},   {RIGHT, ui->rightKeyLabel},
                     {A, ui->aKeyLabel},         {B, ui->bKeyLabel},
                     {START, ui->startKeyLabel}, {SELECT, ui->selectKeyLabel}};

  setKeyButtons = {{UP, ui->setUpButton},       {DOWN, ui->setDownButton},
                   {LEFT, ui->setLeftButton},   {RIGHT, ui->setRightButton},
                   {A, ui->setAButton},         {B, ui->setBButton},
                   {START, ui->setStartButton}, {SELECT, ui->setSelectButton}};

  for (const auto &pair : setKeyButtons) {
    connect(pair.second, &QPushButton::clicked, this,
            [this, pair] { startBind(pair.first); });
  }
}

KeyBindingsWindow::~KeyBindingsWindow() { delete ui; }

void KeyBindingsWindow::startBind(Button button) {
  selectedButton = button;
  acceptKeyPress = true;
  ui->bindStatusLabel->setText("[press any key]");
}

void KeyBindingsWindow::keyPressEvent(QKeyEvent *event) {
  if (acceptKeyPress) {
    controls.bind(event->key(), selectedButton);
    acceptKeyPress = false;
    QKeySequence seq(event->key());
    buttonKeyLabels[selectedButton]->setText(seq.toString());
    ui->bindStatusLabel->setText("[select a button to bind]");
  }
}
