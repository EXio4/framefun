#include "Drawer.h"


namespace Internal {

class Toolkit {
private:
    Drawer& backend;
public:
    Toolkit(Drawer& drawer);

    void line(Pos p1, Pos p2, Color col = Color(255,255,255));
};

}