#pragma once

#include "Drawer.h"
#include <string>
#include <vector>


class FontManager {
private:
    Drawer& backend;
    std::vector<uint8_t> bits[255];
public:
    FontManager(Drawer& backend);
    void write(int x, int y, char c, int scale = 1, Color col = Color(255,255,255), double blend = 1);
    void write(int x, int y, const std::string& string, int scale = 1, int sep = 4, Color col = Color(255,255,255), double blend = 1);
};
