#ifndef GBWIDGET_H
#define GBWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include "gbthread.h"

class GBWidget : public QWidget
{
    Q_OBJECT

public:
    GBWidget(QWidget *parent = nullptr);
    ~GBWidget();

public slots:
    void updateDisplay(const QImage &frame);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap display;
    GBThread gbthread;

signals:
    void sendInput(Joypad button, bool pressed);
};
#endif // GBWIDGET_H
