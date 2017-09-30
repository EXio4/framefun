#include "FontManager.h"



FontManager::FontManager(Drawer& backend) : backend(backend) {
    for (int i=0; i < 256; i++) {
    }
    #include "font.h"
}
bool FontManager::check_bit(int c, int x, int y) {
    if (bits[c].size() == 0) return false;
    if (x < 0 || x >= 16) return false;
    if (y < 0 || y >= 32) return false;
    if (x < 8) {
        return bits[c][y * 2] & (1 << x);
    } else {
        return bits[c][y * 2 + 1] & (1 << (x - 8));
    }
}
void FontManager::write(int x, int y, char c, int scale, Color col, double blend) {
    if (bits[c].size() == 0) return;

    for (int ox = 0; ox < 16; ox++) {
        for (int oy = 0; oy < 32; oy++) {
            if (check_bit(c, ox, oy)) {
                int count = 0;
                for (int Ax = -1; Ax <= 1; Ax++) {
                    for (int Ay = -1; Ay <= 1; Ay++) {
                        count += check_bit(c, ox + Ax, oy + Ay);
                    }
                }

                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        backend.putPixel(x + ox * scale + i, y + oy * scale + j, col, (count < 5 ? 0.5 : 1) * blend);
                    }
                }
            }
        }
    }
}

void FontManager::write(int x, int y, const std::string& string, int scale, int sep, Color col, double blend) {
    for (int i = 0; i < string.size(); i++) {
        write(x + i * (16 * scale + sep), y, string[i], scale, col, blend);
    }
}
