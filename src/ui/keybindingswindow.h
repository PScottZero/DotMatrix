#ifndef KEYBINDINGSWINDOW_H
#define KEYBINDINGSWINDOW_H

#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>

#include "../emulator/controls.h"

namespace Ui {
class KeyBindingsWindow;
}

class KeyBindingsWindow : public QDialog {
  Q_OBJECT

 public:
  explicit KeyBindingsWindow(QWidget *parent = nullptr);
  ~KeyBindingsWindow();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::KeyBindingsWindow *ui;
  Button selectedButton;
  bool acceptKeyPress;
  map<Button, QLabel *> buttonKeyLabels;
  map<Button, QPushButton *> setKeyButtons;

  void startBind(Button button);
};

#endif  // KEYBINDINGSWINDOW_H