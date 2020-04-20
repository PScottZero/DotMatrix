#ifndef DOTMATRIX_PALETTE_H
#define DOTMATRIX_PALETTE_H

#include <QtGui>

// ================================
// Pixel color values
// ================================
constexpr unsigned char PX_ZERO = 0b00;
constexpr unsigned char PX_ONE = 0b01;
constexpr unsigned char PX_TWO = 0b10;
constexpr unsigned char PX_THREE = 0b11;

class Palette : public QObject {
Q_OBJECT

public:
    Palette(uint, uint, uint, uint);
    uint getColor(unsigned char pxVal);

private:
    uint colorZero;
    uint colorOne;
    uint colorTwo;
    uint colorThree;
};

#endif //DOTMATRIX_PALETTE_H
