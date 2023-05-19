#ifndef KEYBINDINGSWINDOW_H
#define KEYBINDINGSWINDOW_H

#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "../emulator/controls.h"

namespace Ui {
class KeyBindingsWindow;
}

class KeyBindingsWindow : public QWidget {
  Q_OBJECT

 public:
  explicit KeyBindingsWindow(Controls *controls, QWidget *parent = nullptr);
  ~KeyBindingsWindow();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private:
  Controls *controls;
  Ui::KeyBindingsWindow *ui;
  Button selectedButton;
  bool acceptKeyPress;
  map<Button, QLabel *> buttonKeyLabels;
  map<Button, QPushButton *> setKeyButtons;

  void startBind(Button button);
};

#endif  // KEYBINDINGSWINDOW_H
