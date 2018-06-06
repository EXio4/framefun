#pragma once

#include <cstdint>
#include "ZFun/Vector.h"

namespace ZFun {

struct ResI {
  using type = int32_t;
};

using Resolution = Vector2<ResI>;

}