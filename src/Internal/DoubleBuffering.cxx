#include "Internal/DoubleBuffering.h"
#include <cstring>

namespace Internal {

DoubleBuffering::DoubleBuffering(std::shared_ptr<RawFB> backend) : backend(backend) {
    ww = backend->screenWidth();
    wh = backend->screenHeight();
    bpp = backend->getBpp();
    lineLen = backend->getLineLen();
    bufferSize = backend->getBufferSize();
    backend_buffer = backend->getRaw();
    local_buffer = std::shared_ptr<uint8_t>(new uint8_t[bufferSize]);
    memcpy((void*)local_buffer.get(), backend_buffer, bufferSize);
}
int DoubleBuffering::screenWidth() { return ww; }
int DoubleBuffering::screenHeight() { return wh; }

void DoubleBuffering::refreshScreen() {
    memcpy(backend_buffer, (void*)local_buffer.get(), bufferSize);
}
void DoubleBuffering::clear() {
    memset(local_buffer.get(), 0, bufferSize);
}


void DoubleBuffering::putPixel(Pos pos, Color newColor, double mix)  {
    if (pos.x < 0 || pos.y < 0 || pos.x >= ww || pos.y >= wh) return;
    uint32_t col = getPixel(pos).blend(newColor, mix).toCode();
    *((uint32_t*)(local_buffer.get()+(pos.x*bpp+pos.y*lineLen)))=col;
}
Color DoubleBuffering::getPixel(Pos pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= ww || pos.y >= wh) return Color(0,0,0);
    return Color(*((uint32_t*)(local_buffer.get()+(pos.x*bpp+pos.y*lineLen))));
}

}