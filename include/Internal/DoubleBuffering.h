#pragma once
#include "Internal/Drawer.h"
#include "Internal/RawFB.h"
#include <memory>
#include <cstdint>

namespace Internal {

class DoubleBuffering : public Drawer {
private:
    std::shared_ptr<uint8_t> local_buffer;
    uint8_t* backend_buffer;
    std::shared_ptr<RawFB> backend;
    int ww;
    int wh;
    uint32_t bpp;
    uint32_t lineLen;
    uint32_t bufferSize;
public:
    DoubleBuffering(std::shared_ptr<RawFB> backend);
    int screenWidth();
    int screenHeight();

    void putPixel(Pos pos, Color col, double mix = 1);
    Color getPixel(Pos pos);
    void refreshScreen();
    void clear();
};

}