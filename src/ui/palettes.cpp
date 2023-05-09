#include "palettes.h"

Palette::Palette(QString name, QString creator, uint color00, uint color01,
                 uint color10, uint color11)
    : name(name), creator(creator), data{color00, color01, color10, color11} {}

vector<Palette *> Palettes::allPalettes = {
    new Palette("2-Bit Demichrome", "Space Sandwich", 0x211E20, 0x555568,
                0xA0A08B, 0xE9EFEC),
    new Palette("Alien GB", "Groxguy", 0xE8FFEE, 0x78DB97, 0x5C5B77, 0x2D293D),
    new Palette("Alien GB Inverted", "Groxguy", 0x2D293D, 0x5C5B77, 0x78DB97,
                0xE8FFEE),
    new Palette("Ayy4", "Polyducks", 0xF1F2DA, 0xFFCE96, 0xFF7777, 0x00303B),
    new Palette("BGB", "BGB Emulator", 0xE0F8D0, 0x88C070, 0x346856, 0x081820),
    new Palette("Bicycle", "Braquen", 0xF0F0F0, 0x8F9BF6, 0xAB4645, 0x161616),
    new Palette("Blk Aqu4", "BlackedIRL", 0x002B59, 0x005F8C, 0x00B9BE,
                0x9FF4E5),
    new Palette("Chocolate", "GrafxKid", 0xFFE4C2, 0xDCA456, 0xA9604C,
                0x422936),
    new Palette("Cobalt", "Me", 0xC5DFFA, 0x5C88B8, 0x1A4778, 0x00234A),
    new Palette("Coldfire GB", "Kerrie Lake", 0xF6C6A8, 0xD17C7C, 0x5B768D,
                0x46425E),
    new Palette("Galaga", "Me", 0x000000, 0xDD0E18, 0x06B6AE, 0xFFFFFF),
    new Palette("Game Boy", "Me", 0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F),
    new Palette("Game Boy Pocket", "Me", 0xFFFFFF, 0xAAAAAA, 0x555555,
                0x000000),
    new Palette("Honey GB", "ANoob", 0x3E3A42, 0x877286, 0xF0B695, 0xE9F5DA),
    new Palette("Ice Cream GB", "Kerrie Lake", 0xFFF6D3, 0xF9A875, 0xEB6B6F,
                0x7C3F58),
    new Palette("Inverted", "Me", 0x000000, 0x545454, 0xA9A9A9, 0xFFFFFF),
    new Palette("Kirby", "SGB", 0xF7BEF7, 0xE78686, 0x7732E7, 0x2D2B96),
    new Palette("Kirokaze Game Boy", "Kirokaze", 0xE2F3E4, 0x94E344, 0x46878F,
                0x332C50),
    new Palette("Link's Awakening", "SGB", 0xFFFFB5, 0x7BC67B, 0x6B8C42,
                0x5A3921),
    new Palette("Mist GB", "Kerrie Lake", 0xC4F0C2, 0x5AB9A8, 0x1E606E,
                0x2D1B00),
    new Palette("Nymph GB", "Kerrie Lake", 0xA1EF8C, 0x3FAC95, 0x446176,
                0x2C2137),
    new Palette("Platinum", "WildLeoKnight", 0xE0F0E8, 0xA8C0B0, 0x4F7868,
                0x183030),
    new Palette("Pokémon", "SGB", 0xFFEFFF, 0xF7B58C, 0x84739C, 0x181010),
    new Palette("Purpledawn", "WildLeoKnight", 0x001B2E, 0x2D757E, 0x9A7BBC,
                0xEEFDED),
    new Palette("Red Is Dead", "Devine Devine", 0x11070A, 0x860020, 0xFF0015,
                0xFFFCFE),
    new Palette("Red Poster", "Nikita Wineberger", 0xE8D6C0, 0x92938D, 0xA1281C,
                0x000000),
    new Palette("Rustic GB", "Kerrie Lake", 0xA96868, 0xEDB4A1, 0x764462,
                0x2C2137),
    new Palette("Slurry GB", "Braquen", 0xFFFFC7, 0xD4984A, 0x4E494C, 0x00303B),
    new Palette("Spacehaze", "WildLeoKnight", 0x0B0630, 0x6B1FB1, 0xCC3495,
                0xF8E3C4),
    new Palette("Super Mario Land 2", "SGB", 0xEFF7B6, 0xDFA677, 0x11C600,
                0x000000),
    new Palette("T-Lollipop", "ANoob", 0x151640, 0x3F6D9E, 0xF783B0, 0xE6F2EF),
    new Palette("Terminal", "Me", 0x000000, 0x005500, 0x00AA00, 0x00FF00),
    new Palette("Virtual Boy", "Me", 0x000000, 0x550000, 0xAA0000, 0xFF0000),
    new Palette("Wish GB", "Kerrie Lake", 0x612F4C, 0x7450E9, 0x5F8FCF,
                0x8BE5FF),
};
