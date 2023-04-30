#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QDialog>

namespace Ui {
class MemoryView;
}

class MemoryView : public QDialog {
  Q_OBJECT

 public:
  explicit MemoryView(QWidget *parent = nullptr);
  ~MemoryView();

 private:
  Ui::MemoryView *ui;
};

#endif  // MEMORYVIEW_H
