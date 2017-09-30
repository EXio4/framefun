#include "Color.h"
#include "Drawer.h"
#include "RawFB.h"
#include "DoubleBuffering.h"

int main(int argc, char ** argv){

    DoubleBuffering fb(std::shared_ptr<RawFB>(new RawFB("/dev/fb0")));

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
                fb.putPixel(x,fb.screenHeight()/2 + y, Color(255,127,255),(RANGE-abs(y))/RANGE);
            }
        }

        fb.refreshScreen();
    }

    return 0;
}
