#include "gbwidget.h"

GBWidget::GBWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Dot Matrix v0.0.0"));
    connect(this, SIGNAL(sendInput(Joypad, bool)), &gbthread, SLOT(processInput(Joypad, bool)));
    connect(&gbthread, SIGNAL(sendFrame(QImage)), this, SLOT(updateDisplay(QImage)));
    resize(640, 576);
    gbthread.start();
}

GBWidget::~GBWidget()
{
    gbthread.exit();
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

void GBWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case KEYCODE_M:
            emit sendInput(START, true);
            break;
        case KEYCODE_N:
            emit sendInput(SELECT, true);
            break;
        case KEYCODE_W:
            emit sendInput(UP, true);
            break;
        case KEYCODE_S:
            emit sendInput(DOWN, true);
            break;
        case KEYCODE_A:
            emit sendInput(LEFT, true);
            break;
        case KEYCODE_D:
            emit sendInput(RIGHT, true);
            break;
        case KEYCODE_P:
            emit sendInput(BUTTON_A, true);
            break;
        case KEYCODE_O:
            emit sendInput(BUTTON_B, true);
            break;
    }
}

void GBWidget::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case KEYCODE_M:
            emit sendInput(START, false);
            break;
        case KEYCODE_N:
            emit sendInput(SELECT, false);
            break;
        case KEYCODE_W:
            emit sendInput(UP, false);
            break;
        case KEYCODE_S:
            emit sendInput(DOWN, false);
            break;
        case KEYCODE_A:
            emit sendInput(LEFT, false);
            break;
        case KEYCODE_D:
            emit sendInput(RIGHT, false);
            break;
        case KEYCODE_P:
            emit sendInput(BUTTON_A, false);
            break;
        case KEYCODE_O:
            emit sendInput(BUTTON_B, false);
            break;
    }
}
