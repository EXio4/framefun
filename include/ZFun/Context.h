#pragma once

#include <memory>
#include "ZFun/Window.h"

namespace ZFun {

class Context {

public:
  std::shared_ptr<ZFun::Window> Window(const std::string& title, const Resolution& res);
  
};


};