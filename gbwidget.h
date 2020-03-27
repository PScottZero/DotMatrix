#ifndef GBWIDGET_H
#define GBWIDGET_H

#include <QWidget>
#include <QPainter>
#include "gbthread.h"

class GBWidget : public QWidget
{
    Q_OBJECT

public:
    GBWidget(QWidget *parent = nullptr);
    ~GBWidget();

public slots:
    void updateDisplay(const QImage &frame);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap display;
    GBThread gbthread;
};
#endif // GBWIDGET_H
