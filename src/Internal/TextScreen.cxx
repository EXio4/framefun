#include "Internal/TextScreen.h"
#include "Internal/Toolkit.h"

namespace Internal {

TextScreen::TextScreen(Drawer& backend, uint32_t blimit) : backend(backend), fm(backend) {
    buffer_limit = blimit;
}

void TextScreen::addLine(const std::string& str) {
    buffers.push_back(str);
    if (buffers.size() > buffer_limit) {
        buffers.pop_front();
    }
}

void TextScreen::setInputPrompt(std::string prompt) {
    this->prompt = prompt;
}

void TextScreen::addInput(char c) {
    if (c == 127) {
        if (input_line.size() > 0) {
            input_line.pop_back();
        }
    } else {
        input_line.push_back(c);
    }
}

void TextScreen::render() {
 Toolkit tk(backend);
    const int factor_width = 8;
    const int factor_height = 16;
    int width = backend.screenWidth() / factor_width;
    int height = backend.screenHeight() / factor_height;
    int y = 0;
    int ix = buffers.size()-height-1;
    if (ix < 0) ix = 0;
    for (int i=0; i < height-1; i++) {
        if (ix+i >= buffers.size()) break;
        int x = 0;
        for (int j = 0; j < width; j++) {
            if (j >= buffers[ix+i].size()) break;
            fm.write(Pos(x,y), buffers[ix+i][j]);
            x += factor_width;
        }
        y += factor_height;
    }
    {
        y = backend.screenHeight() - factor_height;
        tk.line(Pos(0,y), Pos(backend.screenWidth(),y), Color(0,128,0));
        int x = 0;
        for (int i=0; i < prompt.size(); i++) {
            fm.write(Pos(x,y), prompt[i]);
            x += factor_width;
        }
        bool too_long = false;
        int min = 0;
        int max = input_line.size();
        int diff = width - prompt.size() - 1;
        if (max > diff) {
            too_long = true;
            max = diff;
            min = input_line.size() - diff;
        }
        for (int i=min; i < max; i++) {
            fm.write(Pos(x,y), input_line[i]);
            x += factor_width;
        }
    }

}

void TextScreen::clear() {
    buffers.clear();

}

std::string TextScreen::getInput(bool clear) {
    if (clear) {
        std::string ref = input_line;
        input_line.clear();
        return ref;
    } else {
        return input_line;
    }
}

}