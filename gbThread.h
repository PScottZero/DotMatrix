#ifndef GBTHREAD_H
#define GBTHREAD_H

#include <QThread>
#include "cpu.h"
#include "ppu.h"

constexpr auto KEYCODE_W = 87; // up
constexpr auto KEYCODE_A = 65; // left
constexpr auto KEYCODE_S = 83; // down
constexpr auto KEYCODE_D = 68; // right
constexpr auto KEYCODE_M = 77; // start
constexpr auto KEYCODE_N = 78; // select
constexpr auto KEYCODE_O = 79; // b button
constexpr auto KEYCODE_P = 80; // a button

class GBThread: public QThread
{
    Q_OBJECT

public:
    bool emitted;
    CPU cpu;
    PPU *ppu;
    std::string rom;
    int speed;
    explicit GBThread(QObject *parent = nullptr);
    ~GBThread() override;
    void setRom(std::string dir);
    void saveRAM() const;
    void setSpeed(int frameRate);

public slots:
    void processInput(Joypad button, bool pressed);

protected:
    void run() override;

private:
    bool checkBankType();
    bool checkForRAM(unsigned char bankType);
    bool checkForBattery(unsigned char bankType);

signals:
    void sendFrame(const QImage &frame);
    void sendBankError(unsigned char);
};

#endif // GBTHREAD_H
