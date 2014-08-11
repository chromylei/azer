#pragma once

#include <ostream>
#include "azer/math/degree.h"

namespace azer {

class Radians;
class Vector3;
class Vector4;
class Matrix4;

/**
 * Quaternion 是实现绕任意轴旋转的工具
 * == Quaternion 表示旋转 ==
 * Q = [ cos(alpha/2), sin(alphs / 2) * RVec]
 * 其中 alpha 表示旋转角度, RVec 表示旋转轴
 * 旋转运算如下：
 * new_v = Q * v * Q^-1
 *
 */

class Quaternion {
 public:
  Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
  Quaternion(float w, float x, float y, float z) {
    this->w = w; this->x = x; this->y = y; this->z = z; 
  }
  Quaternion(const Quaternion& q)
      : w(q.w), x(q.x), y(q.y), z(q.z) {
  }

  // construct a quaternion from a rotation matrix
  Quaternion(const Matrix4& mat);
  Quaternion(const Vector3& axis, const Radians rad);
  Quaternion(const Vector3& axis, const Degree angle);

  // construct rotation matrix
  Matrix4 ToMatrix() const;
  void ToAxisAngle(Vector3* axis, Degree* degree) const;
  void ToAxisAngle(Vector3* axis, Radians* degree) const;
  void ToAxis(Vector3* axis) const;
  void ToAxis(Vector3* xaxis, Vector3* yaxis, Vector3* zaxis) const;

  Vector3 xaxis() const;
  Vector3 yaxis() const;
  Vector3 zaxis() const;

  static Quaternion FromAxis(const Vector3& xaxis, const Vector3& yaxis,
                             const Vector3& zaxis);
  static Quaternion FromRotate(const Vector3& axis, Radians rad);
  static Quaternion FromMatrix(const Matrix4& mat);

  Quaternion operator + (const Quaternion& q) const;
  Quaternion operator - (const Quaternion& q) const;
  Quaternion operator -() const;

  friend Quaternion operator * (const Quaternion &q, const Quaternion& r);
  Quaternion operator * (float scale) const;

  /* (Q * v * Q^-1) */
  Vector3 operator * (const Vector3& v) const;

  float operator[] (const size_t i) const;
  float& operator[] (const size_t i);
  float* ptr();
  const float* ptr() const;

  friend bool operator == (const Quaternion& q1, const Quaternion& q2);
  bool isnan() const;

  float length() const;
  float dot (const Quaternion& q) const;
  Quaternion Inverse() const;
  Quaternion UnitInverse() const;

  Quaternion Exp() const;
  Quaternion Log() const;

  Quaternion& Normalize();
  Quaternion NormalizeCopy() const;

  /** Performs Spherical linear interpolation between two quaternions,
      and returns the result.
      Slerp ( 0.0f, A, B ) = A
      Slerp ( 1.0f, A, B ) = B
      @return Interpolated quaternion
      @remarks
      Slerp has the proprieties of performing the interpolation at constant
      velocity, and being torque-minimal (unless shortestPath=false).
      However, it's NOT commutative, which means
      Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
      therefore be careful if your code relies in the order of the operands.
      This is specially important in IK animation.
  */
  static Quaternion Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ,
                          bool shortestPath = false);

  /** @see Slerp. It adds extra "spins" (i.e. rotates several times) specified
      by parameter 'iExtraSpins' while interpolating before arriving to the
      final values
  */
  static Quaternion SlerpExtraSpins(float fT,
                                    const Quaternion& rkP, const Quaternion& rkQ,
                                    int iExtraSpins);

  // setup for spherical quadratic interpolation
  static void Intermediate(const Quaternion& rkQ0,
                           const Quaternion& rkQ1, const Quaternion& rkQ2,
                           Quaternion& rka, Quaternion& rkB);
  
  static Quaternion Squad(float fT, const Quaternion& rkP, const Quaternion& rkA,
                          const Quaternion& rkB, const Quaternion& rkQ,
                          bool shortestPath = false);

  /** Performs Normalised linear interpolation between two quaternions,
      and returns the result.
      nlerp ( 0.0f, A, B ) = A
      nlerp ( 1.0f, A, B ) = B
      @remarks
      Nlerp is faster than Slerp.
      Nlerp has the proprieties of being commutative (@see Slerp;
      commutativity is desired in certain places, like IK animation), and
      being torque-minimal (unless shortestPath=false). However, it's performing
      the interpolation at non-constant velocity; sometimes this is desired,
      sometimes it is not. Having a non-constant velocity can produce a more
      natural rotation feeling without the need of tweaking the weights; however
      if your scene relies on the timing of the rotation or assumes it will point
      at a specific angle at a specific weight value, Slerp is a better choice.
  */
  static Quaternion nlerp(float fT, const Quaternion& rkP, 
                          const Quaternion& rkQ, bool shortestPath = false);

  Vector4 AxisX() const;
  Vector4 AxisY() const;
  Vector4 AxisZ() const;

  static const Quaternion kZero;
  static const Quaternion kIdentity;
  static const float kEpsilon;

  friend std::ostream& operator << (std::ostream& os, const Quaternion& q);
 public:
  float w, x, y, z;
};

}  // namespace azer
