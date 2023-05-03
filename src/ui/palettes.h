#pragma once

#include <QObject>

#define PALETTE_COLOR_COUNT 4

class Palette : public QObject {
  Q_OBJECT

 public:
  uint data[PALETTE_COLOR_COUNT];

  Palette(uint color00, uint color01, uint color10, uint color11);
};

class Palettes {
 public:
  static Palette palBGB, palBicycle, palChocolate, palCobalt, palGB, palGBP,
      palInverted, palKirby, palPlatinum, palPokemon, palVB, palWishGB;
};
