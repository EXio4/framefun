#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/mman.h>
#include <string>
#include <iostream>

class Color {
public:
    int r;
    int g;
    int b;
    Color(int r, int g, int b) : r(r), g(g), b(b) {};
    Color(uint32_t c) {
        r = (c & 0xFF0000) >> 16;
        g = (c & 0xFF00) >> 8;
        b = c & 0xFF;
    };
    uint32_t toCode() {
        return (r<<16) + (g<<8) + b;
    }
};

class Drawer {
    virtual void putPixel(int X, int Y, Color col, double mix = 1) = 0;
    virtual Color getPixel(int X, int Y) = 0;
    virtual int screenWidth() = 0;
    virtual int screenHeight() = 0;
};

class RawFB : public Drawer {
private:
    uint8_t *fbp;

    uint16_t ww;
    uint16_t wh;

    uint32_t xoffset, yoffset, bpp, lineLen;
    uint32_t bufferSize;
public:
    RawFB(const std::string& fb_file) {
        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;

        /* inits */
        int fb_fd = open(fb_file.c_str(), O_RDWR);
        ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
        ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
        bufferSize = vinfo.yres_virtual * finfo.line_length;
        fbp = (uint8_t*)mmap(0, bufferSize,
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED, fb_fd, (off_t)0);
        xoffset = vinfo.xoffset;
        yoffset = vinfo.yoffset;
        lineLen = finfo.line_length;
        bpp     = vinfo.bits_per_pixel/8;
        ww      = (uint16_t) vinfo.xres;
        wh      = (uint16_t) vinfo.yres;
    }
    void putPixel(int X, int Y, Color newColor, double mix = 1);
    Color getPixel(int X, int Y);

    int screenWidth() { return ww; }
    int screenHeight() { return wh; }
};

int main(int argc, char ** argv){

    RawFB fb("/dev/fb0");

    for (int x=0; x<fb.screenWidth(); x++) {
        for (int y=0; y<fb.screenHeight(); y++) {
            fb.putPixel(x,y, Color(0,0,0));
        }
    }

    for (int y=-96; y<96; y++) {
        for (int x=0; x<fb.screenWidth(); x++) {
            fb.putPixel(x,fb.screenHeight()/2 + y, Color(255,127,255),(96-abs(y))/96.0);
        }
    }

    return 0;
}


void RawFB::putPixel(int X, int Y, Color newColor, double mix)  {
    if (X < 0 || Y < 0 || X >= ww || Y >= wh) return;
    Color oldColor = getPixel(X,Y);
    Color finalPixel = Color(oldColor.r * (1-mix) + newColor.r * mix,
                             oldColor.g * (1-mix) + newColor.g * mix,
                             oldColor.b * (1-mix) + newColor.b * mix);
    uint32_t col = finalPixel.toCode();
    *((uint32_t*)(fbp+((X+xoffset)*bpp+(Y+yoffset)*lineLen)))=col;
}
Color RawFB::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(fbp+((X+xoffset)*bpp+(Y+yoffset)*lineLen))));
}
