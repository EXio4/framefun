#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/mman.h>

#include <cstring>
#include <iostream>
#include <string>
#include <memory>


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

    uint32_t bpp, lineLen;
    uint32_t bufferSize;
public:
    RawFB(const std::string& fb_file) {
        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;

        /* inits */
        int fb_fd = open(fb_file.c_str(), O_RDWR);
        ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
        ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        ioctl (fb_fd, FBIOPAN_DISPLAY, &vinfo);
        bufferSize = vinfo.yres_virtual * finfo.line_length;
        fbp = (uint8_t*)mmap(0, bufferSize,
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED, fb_fd, (off_t)0);
        lineLen = finfo.line_length;
        bpp     = vinfo.bits_per_pixel/8;
        ww      = (uint16_t) vinfo.xres;
        wh      = (uint16_t) vinfo.yres;
    }
    void putPixel(int X, int Y, Color newColor, double mix = 1);
    Color getPixel(int X, int Y);

    int screenWidth() { return ww; }
    int screenHeight() { return wh; }

    /* awful shit because OOP is sooo good */
    void putRaw(uint8_t* buffer) {
        memcpy(fbp, buffer, bufferSize);
    }
    uint8_t* getRaw() {
        return fbp;
    }
    uint32_t getBpp() { return bpp; }
    uint32_t getLineLen() { return lineLen; }
    uint32_t getBufferSize() { return bufferSize; }
};

class DoubleBuffering : public Drawer {
private:
    std::shared_ptr<uint8_t> local_buffer;
    std::shared_ptr<RawFB> backend;
    int ww;
    int wh;
public:
    DoubleBuffering(std::shared_ptr<RawFB> backend) : backend(backend) {
        ww = backend->screenWidth();
        wh = backend->screenHeight();
        local_buffer = std::shared_ptr<uint8_t>(new uint8_t[backend->getBufferSize()]);
        memcpy((void*)local_buffer.get(), (void*)backend->getRaw(), backend->getBufferSize());
    }
    int screenWidth() { return ww; }
    int screenHeight() { return wh; }

    void putPixel(int X, int Y, Color col, double mix = 1);
    Color getPixel(int X, int Y);
    void refreshScreen() {
        memcpy((void*)backend->getRaw(), (void*)local_buffer.get(), backend->getBufferSize());
    }
    void clear() {
        memset(local_buffer.get(), 0, backend->getBufferSize());
    }
};

int main(int argc, char ** argv){

    DoubleBuffering fb(std::shared_ptr<RawFB>(new RawFB("/dev/fb0")));

    bool up = true;
    int c = -16;
    while (1) {

        if (c >=  16) { up = false; }
        if (c <= -18) { up = true;  }

        c += up ? 1 : -1;

        fb.clear();

        const double RANGE = abs(c) * 4;
        for (int y=-RANGE; y<RANGE; y++) {
            for (int x=0; x<fb.screenWidth(); x++) {
                fb.putPixel(x,fb.screenHeight()/2 + y, Color(255,127,255),(RANGE-abs(y))/RANGE);
            }
        }

        fb.refreshScreen();
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
    *((uint32_t*)(fbp+(X*bpp+Y*lineLen)))=col;
}
Color RawFB::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(fbp+(X*bpp+Y*lineLen))));
}


// this is far from efficient, caching bpp/linelen is going to be much faster

void DoubleBuffering::putPixel(int X, int Y, Color newColor, double mix)  {
    if (X < 0 || Y < 0 || X >= ww || Y >= wh) return;
    Color oldColor = getPixel(X,Y);
    Color finalPixel = Color(oldColor.r * (1-mix) + newColor.r * mix,
                             oldColor.g * (1-mix) + newColor.g * mix,
                             oldColor.b * (1-mix) + newColor.b * mix);
    uint32_t col = finalPixel.toCode();
    *((uint32_t*)(local_buffer.get()+(X*backend->getBpp()+Y*backend->getLineLen())))=col;
}
Color DoubleBuffering::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(local_buffer.get()+(X*backend->getBpp()+Y*backend->getLineLen()))));
}
