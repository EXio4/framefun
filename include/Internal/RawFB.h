#pragma once
#include "Internal/Drawer.h"
#include <string>


namespace Internal {

class RawFB : public Drawer {
private:
    uint8_t *fbp;

    uint16_t ww;
    uint16_t wh;

    uint32_t bpp, lineLen;
    uint32_t bufferSize;
public:
    RawFB(const std::string& fb_file);
    void putPixel(Pos pos, Color newColor, double mix = 1);
    Color getPixel(Pos pos);

    int screenWidth();
    int screenHeight();

    /* awful shit because OOP is sooo good */
    void putRaw(uint8_t* buffer);
    uint8_t* getRaw();
    uint32_t getBpp();
    uint32_t getLineLen();
    uint32_t getBufferSize();
};

}