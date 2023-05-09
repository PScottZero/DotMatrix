#pragma once

#include <QObject>
#include <vector>

#define PALETTE_COLOR_COUNT 4
#define DEFAULT_PALETTE_IDX 12

using namespace std;

class Palette : public QObject {
  Q_OBJECT

 public:
  QString name, creator;
  uint data[PALETTE_COLOR_COUNT];

  Palette(QString name, QString creator, uint color00, uint color01,
          uint color10, uint color11);
};

class Palettes {
 public:
  static vector<Palette *> allPalettes;
};
