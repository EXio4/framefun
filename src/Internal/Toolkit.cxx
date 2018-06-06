#include "Internal/Toolkit.h"
#include <math.h>
#include <algorithm>

namespace Internal {

Toolkit::Toolkit(Drawer& backend) : backend(backend) {};

// stolen from Rosetta Code
void Toolkit::line(Pos p0, Pos p1, Color col) {
    double x1 = p0.x;
    double x2 = p1.x;
    double y1 = p0.y;
    double y2 = p1.y;
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if(x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const double dx = x2 - x1;
    const double dy = fabs(y2 - y1);

    double error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++) {
        if(steep) {
            backend.putPixel(Pos(y,x),col);
        } else {
            backend.putPixel(Pos(x,y),col);
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
};

}