#include "palette.h"

Palette::Palette(uint palZero, uint palOne, uint palTwo, uint palThree) : QObject() {
    colorZero = palZero;
    colorOne = palOne;
    colorTwo = palTwo;
    colorThree = palThree;
}

uint Palette::getColor(unsigned char pxVal) {
    switch (pxVal) {
        case PX_ZERO:
            return colorZero;
        case PX_ONE:
            return colorOne;
        case PX_TWO:
            return colorTwo;
        case PX_THREE:
            return colorThree;
        default:
            return 0;
    }
}


