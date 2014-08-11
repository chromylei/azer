#include "azer/math/quaternion.h"

namespace azer {

inline Vector3::Vector3() {
  this->x = 0.0f;
  this->y = 0.0f;
  this->z = 0.0f;
}

inline Vector3::Vector3(float x, float  y, float z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

inline Vector3::Vector3(const Vector3& vec) {
  this->x = vec.x;
  this->y = vec.y;
  this->z = vec.z;
}

inline Vector3::Vector3(const Vector4& vec) {
  this->x = vec.x;
  this->y = vec.y;
  this->z = vec.z;
}

inline bool operator == (const Vector3& v1, const Vector3& v2) {
  return (std::abs(v1.x - v2.x) < kFloatMargin)
      && (std::abs(v1.y - v2.y) < kFloatMargin)
      && (std::abs(v1.z - v2.z) < kFloatMargin);
}

inline float Vector3::dot(const Vector3& vec) const {
  return vec.x * x + vec.y * y + vec.z * z;
}

inline float Vector3::distance(const Vector3& vec) const {
  return azer::sqrt((x  - vec.x) * (x  - vec.x) +
                    (y  - vec.y) * (y  - vec.y) +
                    (z  - vec.z) * (z  - vec.z));
}

inline float Vector3::length() const {
  return std::sqrt(dot(*this));
}

inline Vector3 Vector3::operator - () const {
  return Vector3(-x, -y, -z);
}

inline Vector3& Vector3::operator += (const Vector3& vec) {
  this->x += vec.x;
  this->y += vec.y;
  this->z += vec.z;
  return *this;
}

inline Vector3& Vector3::operator -= (const Vector3& vec) {
  this->x -= vec.x;
  this->y -= vec.y;
  this->z -= vec.z;
  return *this;
}

inline Vector3& Vector3::operator += (const float scalar) {
  this->x += scalar;
  this->y += scalar;
  this->z += scalar;
  return *this;
}

inline Vector3& Vector3::operator -= (const float scalar) {
  this->x -= scalar;
  this->y -= scalar;
  this->z -= scalar;
  return *this;
}

inline  Vector3& Vector3::operator *= (const float scalar) {
  this->x *= scalar;
  this->y *= scalar;
  this->z *= scalar;
  return *this;
}

inline Vector3& Vector3::operator /= (const float scalar) {
  this->x /= scalar;
  this->y /= scalar;
  this->z /= scalar;
  return *this;
}

inline Vector3& Vector3::Normalize() {
  float len = length();
  // Will also work for zero-sized vectors, but will change nothing
  // We're not using epsilons because we don't need to.
  // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
  if (len > 0) {
    float invlen = 1 / len;
    *this *= invlen;
  }

return *this;
}

inline Vector3 Vector3::NormalizeCopy() const {
  Vector3 vec(*this);
  return vec.Normalize();
}

inline Vector3 Vector3::cross(const Vector3& vec) const {
  return Vector3(
      y * vec.z - z * vec.y,
      z * vec.x - x * vec.z,
      x * vec.y - y * vec.x);
}

inline Vector3 CrossProduct(const Vector3 v1, const Vector3 v2) {
  return v1.cross(v2);
}

inline Vector3 operator *(const Vector3& v, const float scalar) {
  return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

inline Vector3 operator /(const Vector3& v, const float scalar) {
  return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
}

inline Vector3 operator + (const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3 operator - (const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vector3& Vector3::operator = (const Vector3 & vec) {
  this->x = vec.x; this->y = vec.y; this->z = vec.z;
  return *this;
}

inline Vector3& Vector3::operator = (const Vector4 & vec) {
  this->x = vec.x; this->y = vec.y; this->z = vec.z;
  return *this;
}

inline Vector3 Vector3::RotateCopy(const Vector3& axis, const Radians rad) const {
  Quaternion Q(axis, rad);
  return Q * *this;
}

inline void Vector3::Rotate(const Vector3& axis, const Radians rad) {
  *this = Vector3::RotateCopy(axis, rad);
}

inline void Vector3::Rotate(const Vector3& axis, const Degree ang) {
  Vector3::Rotate(axis, Radians(ang));
}

inline Vector3 Vector3::RotateCopy(const Vector3& axis, const Degree ang) const {
  return Vector3::RotateCopy(axis, Radians(ang));
}

inline std::ostream& operator << (std::ostream& os, const Vector3& v) {
  os << std::setprecision (8) << "vec3("
     << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

}  // namespace azer
