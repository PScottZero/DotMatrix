#ifndef GBTHREAD_H
#define GBTHREAD_H

#include <QThread>
#include "cpu.h"
#include "ppu.h"

class GBThread: public QThread
{
    Q_OBJECT

public:
    GBThread(QObject *parent = nullptr);
    ~GBThread();

protected:
    void run() override;

private:
    bool emitted;

signals:
    void sendFrame(const QImage &frame);
};

#endif // GBTHREAD_H
