#pragma once
#include <cstdint>

namespace Internal {

class Color {
public:
    int r;
    int g;
    int b;
    Color(int r, int g, int b);
    Color(uint32_t c);
    uint32_t toCode();
    Color blend(Color other, double mix);
};

}