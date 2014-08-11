#pragma once

#include <cmath>

namespace azer {
class Matrix4;
class Vector3;

// [x, y, z, w]'
// [r, g, b, a]'
// []'
class Vector4 {
 public:
  Vector4();
  Vector4(float x, float  y, float z, float w);
  Vector4(const Vector4& vec);
  Vector4(const Vector3& vec, float w);

  float r() const { return x;}
  float g() const { return y;}
  float b() const { return z;}
  float a() const { return w;}

  float length() const;
  const float& operator[](int index) const;
  float& operator[](int index);

  friend Vector4 operator *(const Vector4& v, const float scalar);
  friend Vector4 operator /(const Vector4& v, const float scalar);
  friend Vector4 operator + (const Vector4& v1, const Vector4& v2);
  friend Vector4 operator - (const Vector4& v1, const Vector4& v2);

  friend bool operator == (const Vector4& v1, const Vector4& v2);
  Vector4 operator - () const;
  Vector4& operator += (const Vector4& vec);
  Vector4& operator -= (const Vector4& vec);
  Vector4& operator += (const float scalar);
  Vector4& operator -= (const float scalar);
  Vector4& operator *= (const float scalar);
  Vector4& operator /= (const float scalar);

  float dot(const Vector4& vec) const;
  Vector4& Normalize();
  Vector4 NormalizeCopy() const;

  float* Pointer() { return &x;}
  const float* Pointer() const { return &x;}

  static const Vector4 kZero;
 public:
  float x, y, z, w;
  friend class Matrix4;
};

}  // namespace azer
