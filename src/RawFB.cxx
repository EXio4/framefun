#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/mman.h>
#include "RawFB.h"
#include <cstring>

RawFB::RawFB(const std::string& fb_file) {
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

int RawFB::screenWidth() { return ww; }
int RawFB::screenHeight() { return wh; }

/* awful shit because OOP is sooo good */
void RawFB::putRaw(uint8_t* buffer) {
    memcpy(fbp, buffer, bufferSize);
}
uint8_t* RawFB::getRaw() {
    return fbp;
}
uint32_t RawFB::getBpp() { return bpp; }
uint32_t RawFB::getLineLen() { return lineLen; }
uint32_t RawFB::getBufferSize() { return bufferSize; }

void RawFB::putPixel(Pos pos, Color newColor, double mix)  {
    if (pos.x < 0 || pos.y < 0 || pos.x >= ww || pos.y >= wh) return;
    uint32_t col = getPixel(pos).blend(newColor, mix).toCode();
    *((uint32_t*)(fbp+(pos.x*bpp+pos.y*lineLen)))=col;
}
Color RawFB::getPixel(Pos pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= ww || pos.y >= wh) return Color(0,0,0);
    return Color(*((uint32_t*)(fbp+(pos.x*bpp+pos.y*lineLen))));
}
