#pragma once

#include "Drawer.h"
#include <string>
#include <vector>

#define FONT_HEIGHT 16
#define FONT_WIDTH 8

class FontManager {
private:
    Drawer& backend;
    std::vector<uint8_t> bits[1024];
    bool check_bit(int c, int x, int y);
public:
    FontManager(Drawer& backend);
    void write(Pos pos, char c, int scale = 1, Color col = Color(255,255,255), double blend = 1);
    void write(Pos pos, const std::string& string, int scale = 1, int sep = 4, Color col = Color(255,255,255), double blend = 1);
};
