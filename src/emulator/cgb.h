#pragma once

#include <QAction>
#include <QImage>
#include <QThread>

#define FRAME_RATE 59.7275
#define US_PER_SEC 1000000.0
#define FRAME_DURATION US_PER_SEC / 59.7275

class CGB : public QThread {
  Q_OBJECT

 private:
  QImage screen;

 public:
  QAction *actionPause;
  static bool stop;
  float speedMult;
  bool pause;

  CGB();
  ~CGB();

  void run() override;
  void reset();
  bool loadROM(const QString dir);

 signals:
  void sendScreen(QImage);
};
