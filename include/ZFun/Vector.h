#pragma once

#include <cstdint>

namespace ZFun {

template <typename T>
struct Vector2 {
  typename T::type x;
  typename T::type y;

  Vector2(typename T::type x, typename T::type y) : x(x), y(y) {};
  Vector2(typename T::type a) : x(a), y(a) {};
  Vector2() : x(0), y(0) {};

};


template <typename T>
Vector2<T> operator+(const Vector2<T>& a, const Vector2<T>& b) {
  return Vector2<T>(a.x + b.x, a.y + b.y);
}

template <typename T>
Vector2<T> operator-(const Vector2<T>& a, const Vector2<T>& b) {
  return Vector2<T>(a.x - b.x, a.y - b.y);
}

template <typename T>
Vector2<T>& operator+=(Vector2<T>& self, const Vector2<T>& other) {
  self.x += other.x;
  self.y += other.y;
}

template <typename T>
Vector2<T>& operator-=(Vector2<T>& self, const Vector2<T>& other) {
  self.x -= other.x;
  self.y -= other.y;
}




struct Vi {
  using type = int32_t;
};
struct Vf {
  using type = double;
};

using Vector2i = Vector2<Vi>;
using Vector2f = Vector2<Vf>;

}