#pragma once
#include "Internal/Color.h"
#include "Internal/Pos.h"


namespace Internal {

class Drawer {
public:
    virtual void putPixel(Pos pos, Color col, double mix = 1) = 0;
    virtual Color getPixel(Pos pos) = 0;
    virtual int screenWidth() = 0;
    virtual int screenHeight() = 0;
};

}