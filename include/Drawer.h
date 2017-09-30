#pragma once
#include "Color.h"

class Drawer {
public:
    virtual void putPixel(int X, int Y, Color col, double mix = 1) = 0;
    virtual Color getPixel(int X, int Y) = 0;
    virtual int screenWidth() = 0;
    virtual int screenHeight() = 0;
};
