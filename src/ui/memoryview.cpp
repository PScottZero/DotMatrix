#include "memoryview.h"

#include "ui_memoryview.h"

MemoryView::MemoryView(QWidget *parent)
    : QDialog(parent), ui(new Ui::MemoryView) {
  ui->setupUi(this);
}

MemoryView::~MemoryView() { delete ui; }
