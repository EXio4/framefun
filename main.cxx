#include <iostream>
#include "Color.h"
#include "Drawer.h"
#include "RawFB.h"
#include "DoubleBuffering.h"
#include "FontManager.h"
#include "Toolkit.h"
#include "TextScreen.h"

int main(int argc, char ** argv){

    std::shared_ptr<RawFB> rawfb = std::shared_ptr<RawFB>(new RawFB("/dev/fb0"));
    DoubleBuffering fb(rawfb);
    FontManager fonts(fb);
    Toolkit tk(fb);
    TextScreen tm(fb, 80);
/*
    bool up = true;
    int c = -16;
    while (1) {

        if (c >=  16) { up = false; }
        if (c <= -18) { up = true;  }

        c += up ? 1 : -1;

        fb.clear();

        const double RANGE = abs(c) * 4;
        for (int y=-RANGE; y<RANGE; y++) {
            for (int x=0; x<fb.screenWidth(); x++) {
                fb.putPixel(Pos(x,fb.screenHeight()/2 + y), Color(255,127,255),(RANGE-abs(y))/RANGE);
            }
        }

        fonts.write(Pos(64,64), "Hello world!", 4);

        tk.line(Pos(320,32), Pos(500,400),Color(0,255,0));
        tk.line(Pos(17, 0), Pos(30, 300),Color(255,0,0));


        fb.refreshScreen();
    }
*/
    int i = 0;
    int acc = 0;
    
    tm.setInputPrompt("exio4@localhost:~$");
    while (1) {
        
        fb.clear();
        
        
        if (i == 0) {
            acc++;
            tm.addLine("Hello, from line " + std::to_string(acc));
        }
        
        i = (i + 1) % 15;
        
        tm.render();
        
        fb.refreshScreen();
        
    }
    return 0;
}
