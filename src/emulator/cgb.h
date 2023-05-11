// **************************************************
// **************************************************
// **************************************************
// GAME BOY COLOR (CURRENTLY ONLY SUPPORTS DMG)
// **************************************************
// **************************************************
// **************************************************

#pragma once

#include <QAction>
#include <QImage>
#include <QThread>

#include "../ui/palettes.h"

#define FRAME_RATE 59.7275
#define US_PER_SEC 1000000.0
#define FRAME_DURATION US_PER_SEC / 59.7275

class CGB : public QThread {
  Q_OBJECT

 private:
  QImage screen;

 public:
  static QString romPath;
  QAction *actionPause;
  static bool stop, dmgMode;
  bool running, pause;
  Palette *tempPalette;

  CGB();
  ~CGB();

  void run() override;
  void reset(bool newGame = true);
  bool loadRom(const QString romPath);
  void renderInPauseMode();

 signals:
  void sendScreen(QImage *screen);

 public slots:
  void previewPalette(Palette *palette);
  void resetPreviewPalette();
};
