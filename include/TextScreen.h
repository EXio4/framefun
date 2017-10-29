#pragma once

#include "FontManager.h"

#include <string>
#include <deque>

class TextScreen {
    std::deque<std::string> buffers;
    uint32_t buffer_limit;
    std::string prompt;
    std::string input_line; // for commands and shit
    Drawer& backend;
    FontManager fm;
public:
    TextScreen(Drawer& backend, uint32_t buffer_limit);
    void addLine(const std::string& str);
    void setInputPrompt(std::string prompt);
    // -1 (equiv) is "backspace", other chars are passthru (will end up as spaces when shown
    // -x (equiv) need to represent left/right etc
    void addInput(uint16_t c);
    void render();
};
