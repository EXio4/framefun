#include "DoubleBuffering.h"
#include <cstring>

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


void DoubleBuffering::putPixel(int X, int Y, Color newColor, double mix)  {
    if (X < 0 || Y < 0 || X >= ww || Y >= wh) return;
    uint32_t col = getPixel(X,Y).blend(newColor, mix).toCode();
    *((uint32_t*)(local_buffer.get()+(X*bpp+Y*lineLen)))=col;
}
Color DoubleBuffering::getPixel(int X, int Y) {
    return Color(*((uint32_t*)(local_buffer.get()+(X*bpp+Y*lineLen))));
}
