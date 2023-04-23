#pragma once

#include <QObject>

#include "types.h"

#define PALETTE_COLOR_COUNT 4

class Palette : public QObject {
  Q_OBJECT

 public:
  uint32 data[PALETTE_COLOR_COUNT];

  Palette(uint32 color00, uint32 color01, uint32 color10, uint32 color11);
};
