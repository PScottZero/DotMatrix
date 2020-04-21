#include "displayWidget.h"

DisplayWidget::DisplayWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(640, 576);
}

DisplayWidget::~DisplayWidget() = default;

void DisplayWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.scale(4, 4);
    if (!display.isNull()) {
        painter.drawPixmap(0, 0, display);
    }
}

void DisplayWidget::updateDisplay(const QImage &frame) {
    display = QPixmap::fromImage(frame);
    update();
}