#pragma once

#include <cstdint>

namespace ZFun {

struct Color {

  int8_t r;
  int8_t g;
  int8_t b;
  int8_t a;

  Color() : r(0), g(0), b(0), a(255) {};
  Color(int8_t all) : r(all), g(all), b(all), a(255) {};
  Color(int8_t r, int8_t g, int8_t b) : r(r), g(g), b(b), a(255) {};
  Color(int8_t r, int8_t g, int8_t b, int8_t a) : r(r), g(g), b(b), a(a) {};

};

}