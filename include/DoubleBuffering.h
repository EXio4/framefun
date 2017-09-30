#pragma once
#include "Drawer.h"
#include "RawFB.h"
#include <memory>
#include <cstdint>

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

    void putPixel(int X, int Y, Color col, double mix = 1);
    Color getPixel(int X, int Y);
    void refreshScreen();
    void clear();
};
