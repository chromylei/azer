#pragma once

#include <ostream>
#include <cmath>
#include <utility>

namespace azer {

class Radians;
class Degree;
class Vector4;

class Vector3 {
 public:
  
  Vector3();
  Vector3(float x, float  y, float z);
  Vector3(const Vector3& vec);
  Vector3(const Vector4& vec);
 public:
  float length() const;

  friend Vector3 operator *(const Vector3& v, const float scalar);
  friend Vector3 operator /(const Vector3& v, const float scalar);
  friend Vector3 operator + (const Vector3& v1, const Vector3& v2);
  friend Vector3 operator - (const Vector3& v1, const Vector3& v2);

  Vector3 operator - () const;
  Vector3& operator += (const Vector3& vec);
  Vector3& operator -= (const Vector3& vec);
  Vector3& operator += (const float scalar);
  Vector3& operator -= (const float scalar);
  Vector3& operator *= (const float scalar);
  Vector3& operator /= (const float scalar);
  Vector3& operator = (const Vector3 & vec);
  Vector3& operator = (const Vector4 & vec);

  friend bool operator == (const Vector3& v1, const Vector3& v2);

  float dot(const Vector3& vec) const;
  float distance(const Vector3& vec) const;
  Vector3 cross(const Vector3& vector) const;
  friend Vector3 CrossProduct(const Vector3 v1, const Vector3 v2);
  Vector3& Normalize();
  Vector3 NormalizeCopy() const;

  static const Vector3& kZero;
  static const Vector3& kUnitX;
  static const Vector3& kUnitY;
  static const Vector3& kUnitZ;

  // rotate
  void Rotate(const Vector3& axis, const Radians rad);
  void Rotate(const Vector3& axis, const Degree ang);
  Vector3 RotateCopy(const Vector3& axis, const Radians rad) const;
  Vector3 RotateCopy(const Vector3& axis, const Degree ang) const;

  friend std::ostream& operator << (std::ostream& o, const Vector3& v);
 public:
  float x, y, z;

  friend class Quaternion;
};

}  // namespace azer
