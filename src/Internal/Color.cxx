#include "Internal/Color.h"

namespace Internal {

Color::Color(int r, int g, int b) : r(r), g(g), b(b) {};
Color::Color(uint32_t c) {
    r = (c & 0xFF0000) >> 16;
    g = (c & 0xFF00) >> 8;
    b = c & 0xFF;
};

uint32_t Color::toCode() {
    return (r<<16) + (g<<8) + b;
}

Color Color::blend(Color other, double mix) {
    return Color(r * (1-mix) + other.r * mix,
                 g * (1-mix) + other.g * mix,
                 b * (1-mix) + other.b * mix);
}

}