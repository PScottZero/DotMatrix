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
  explicit KeyBindingsWindow(Controls &controls, QWidget *parent = nullptr);
  ~KeyBindingsWindow();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private:
  Ui::KeyBindingsWindow *ui;
  Controls &controls;
  Button selectedButton;
  bool acceptKeyPress;
  std::map<Button, QLabel *> buttonKeyLabels;
  std::map<Button, QPushButton *> setKeyButtons;

  void startBind(Button button);
};

#endif  // KEYBINDINGSWINDOW_H
