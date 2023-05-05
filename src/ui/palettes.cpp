#include "palettes.h"

Palette::Palette(QString name, QString creator, uint color00, uint color01,
                 uint color10, uint color11)
    : name(name), creator(creator), data{color00, color01, color10, color11} {}

vector<Palette *> Palettes::allPalettes = {
    new Palette("BGB", "BGB Emulator", 0xE0F8D0, 0x88C070, 0x346856, 0x081820),
    new Palette("Bicycle", "Braquen", 0xF0F0F0, 0x8F9BF6, 0xAB4645, 0x161616),
    new Palette("Chocolate", "GrafxKid", 0xFFE4C2, 0xDCA456, 0xA9604C,
                0x422936),
    new Palette("Cobalt", "Me", 0xC5DFFA, 0x5C88B8, 0x1A4778, 0x00234A),
    new Palette("Game Boy", "Me", 0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F),
    new Palette("Game Boy Pocket", "Me", 0xFFFFFF, 0xAAAAAA, 0x555555,
                0x000000),
    new Palette("Inverted", "Me", 0x000000, 0x545454, 0xA9A9A9, 0xFFFFFF),
    new Palette("Kirby", "SGB", 0xF7BEF7, 0xE78686, 0x7732E7, 0x2D2B96),
    new Palette("Platinum", "WildLeoKnight", 0xE0F0E8, 0xA8C0B0, 0x4F7868,
                0x183030),
    new Palette("Pokémon", "SGB", 0xFFEFFF, 0xF7B58C, 0x84739C, 0x181010),
    new Palette("Virtual Boy", "Me", 0x000000, 0x550000, 0xAA0000, 0xFF0000),
    new Palette("Wish GB", "Kerrie Lake", 0x612F4C, 0x7450E9, 0x5F8FCF,
                0x8BE5FF),
};
