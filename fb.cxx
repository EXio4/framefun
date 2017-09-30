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

    Color blend(Color other, double mix) {
        return Color(r * (1-mix) + other.r * mix,
                     g * (1-mix) + other.g * mix,
                     b * (1-mix) + other.b * mix);
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
    uint8_t* backend_buffer;
    std::shared_ptr<RawFB> backend;
    int ww;
    int wh;
    uint32_t bpp;
    uint32_t lineLen;
    uint32_t bufferSize;
public:
    DoubleBuffering(std::shared_ptr<RawFB> backend) : backend(backend) {
        ww = backend->screenWidth();
        wh = backend->screenHeight();
        bpp = backend->getBpp();
        lineLen = backend->getLineLen();
        bufferSize = backend->getBufferSize();
        backend_buffer = backend->getRaw();
        local_buffer = std::shared_ptr<uint8_t>(new uint8_t[bufferSize]);
        memcpy((void*)local_buffer.get(), backend_buffer, bufferSize);
    }
    int screenWidth() { return ww; }
    int screenHeight() { return wh; }

    void putPixel(int X, int Y, Color col, double mix = 1);
    Color getPixel(int X, int Y);
    void refreshScreen() {
        memcpy(backend_buffer, (void*)local_buffer.get(), bufferSize);
    }
    void clear() {
        memset(local_buffer.get(), 0, bufferSize);
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
    uint32_t col = getPixel(X,Y).blend(newColor, mix).toCode();
    *((uint32_t*)(fbp+(X*bpp+Y*lineLen)))=col;
}
Color RawFB::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(fbp+(X*bpp+Y*lineLen))));
}


void DoubleBuffering::putPixel(int X, int Y, Color newColor, double mix)  {
    if (X < 0 || Y < 0 || X >= ww || Y >= wh) return;
    uint32_t col = getPixel(X,Y).blend(newColor, mix).toCode();
    *((uint32_t*)(local_buffer.get()+(X*bpp+Y*lineLen)))=col;
}
Color DoubleBuffering::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(local_buffer.get()+(X*bpp+Y*lineLen))));
}
