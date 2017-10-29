#include "FontManager.h"



FontManager::FontManager(Drawer& backend) : backend(backend) {
    #include "font.h"
}
bool FontManager::check_bit(int c, int x, int y) {
    if (bits[c].size() == 0) return false;
    if (x < 0 || x >= FONT_WIDTH) return false;
    if (y < 0 || y >= FONT_HEIGHT) return false;
    
    return bits[c][y] & (1 << x);
}
void FontManager::write(Pos pos, char c, int scale, Color col, double blend) {
    if (bits[c].size() == 0) return;

    for (int ox = 0; ox < FONT_WIDTH; ox++) {
        for (int oy = 0; oy < FONT_HEIGHT; oy++) {
            if (check_bit(c, ox, oy)) {
                int count = 0;
                for (int Ax = -1; Ax <= 1; Ax++) {
                    for (int Ay = -1; Ay <= 1; Ay++) {
                        count += check_bit(c, ox + Ax, oy + Ay);
                    }
                }

                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        backend.putPixel(Pos(pos.x + ox * scale + i, pos.y + oy * scale + j), col, blend);
                    }
                }
            }
        }
    }
}

void FontManager::write(Pos pos, const std::string& string, int scale, int sep, Color col, double blend) {
    for (int i = 0; i < string.size(); i++) {
        write(Pos(pos.x + i * (16 * scale + sep), pos.y), string[i], scale, col, blend);
    }
}
