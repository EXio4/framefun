
#include "ZFun/Context.h"
#include "ZFun/Window.h"

int demo_main(ZFun::Context ctx) {

  std::shared_ptr<ZFun::Window> w = ctx.Window("Hello", ZFun::Resolution(640, 480));

  int idx = 0;
  while (w->isOpen()) {
      w->clear();
      for (int i=0; i < 50; i++) {
        w->putPixel(ZFun::Vector2i(idx + i, i), ZFun::Color(5 * i, 5 * i, 5 * i));
      }
      w->render();
  }

}
