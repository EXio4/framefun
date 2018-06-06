#pragma once

#include "ZFun/Resolution.h"
#include "ZFun/Vector.h"
#include "ZFun/Color.h"

namespace ZFun {

class Window {
private:
  Window();
public:

  bool isOpen();
  Resolution getResolution();

  void clear();
  void render();

  void putPixel(const Vector2i &pos, const Color& col);


};

}