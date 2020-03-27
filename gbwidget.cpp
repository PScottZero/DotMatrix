#include "gbwidget.h"

GBWidget::GBWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Dot Matrix v0.0.0"));
    connect(&gbthread, SIGNAL(sendFrame(QImage)), this, SLOT(updateDisplay(QImage)));
    resize(640, 576);
}

GBWidget::~GBWidget()
{

}

void GBWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.scale(4, 4);
    if (!display.isNull()) {
        painter.drawPixmap(0, 0, display);
    }
}

void GBWidget::updateDisplay(const QImage &frame) {
    display = QPixmap::fromImage(frame);
    update();
}
