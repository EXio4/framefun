#include "FontManager.h"



FontManager::FontManager(Drawer& backend) : backend(backend) {
    for (int i=0; i < 256; i++) {
    }
    #include "font.h"
}
void FontManager::write(int x, int y, char c, int scale, Color col, double blend) {

    if (bits[c].size() == 0) return;

    for (int ox = 0; ox < 16; ox++) {
        for (int oy = 0; oy < 32; oy++) {
            bool bit = false;
            if (ox < 8) {
                bit = bits[c][oy * 2] & (1 << ox);
            } else {
                bit = bits[c][oy * 2 + 1] & (1 << (ox - 8));
            }
            if (bit) {
                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        backend.putPixel(x + ox * scale + i, y + oy * scale + j, col, blend);
                    }
                }
            }
        }
    }
}

void FontManager::write(int x, int y, const std::string& string, int scale, int sep, Color col, double blend) {
    for (int i = 0; i < string.size(); i++) {
        write(x + i * (16 + sep), y, string[i], scale, col, blend);
    }
}
