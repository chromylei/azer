#pragma once

namespace azer {

class Vector2 {
 public:
  Vector2();
  Vector2(float x, float y);
  explicit Vector2(float arr[2]);
  Vector2(const Vector2& v);
  
  Vector2 operator - (const Vector2 &v) const;
 public:
  float x, y;
};

inline Vector2::Vector2() : x(0.0f), y(0.0f) {}
inline Vector2::Vector2(float p1, float p2) : x(p1), y(p2) {}
inline Vector2::Vector2(float arr[2]) : x(*arr), y(*(arr + 1)) {}
inline Vector2::Vector2(const Vector2& v) : x(v.x), y(v.y) {}
inline Vector2 Vector2::operator - (const Vector2 &v) const {
  return Vector2(x - v.x, y - v.y);
}
}  // namespace azer
