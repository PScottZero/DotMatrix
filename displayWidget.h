#ifndef GBWIDGET_H
#define GBWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include "gbThread.h"

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    GBThread gbthread;
    explicit DisplayWidget(QWidget *parent = nullptr);
    ~DisplayWidget() override;

public slots:
    void updateDisplay(const QImage &frame);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap display;
};
#endif // GBWIDGET_H
