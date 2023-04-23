#ifndef KEYBINDINGSWINDOW_H
#define KEYBINDINGSWINDOW_H

#include <QDialog>

namespace Ui {
class KeyBindingsWindow;
}

class KeyBindingsWindow : public QDialog {
  Q_OBJECT

 public:
  explicit KeyBindingsWindow(QWidget *parent = nullptr);
  ~KeyBindingsWindow();

 private:
  Ui::KeyBindingsWindow *ui;
};

#endif  // KEYBINDINGSWINDOW_H
