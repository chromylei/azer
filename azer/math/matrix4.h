#pragma once

#include <stdlib.h>
#include <string.h>
#include <utility>
#include <ostream>

namespace azer {

class Radians;
class Degree;
class Vector3;
class Vector4;

// layout consistent with common
class Matrix4 {
 public:
  Matrix4() {
    memset(m_, 0, sizeof(m_));
  }

  explicit Matrix4(const float v[16]);
  explicit Matrix4(float v[16]);
  Matrix4(float _00, float _01, float _02, float _03,
          float _10, float _11, float _12, float _13,
          float _20, float _21, float _22, float _23,
          float _30, float _31, float _32, float _33);

  Matrix4(const Vector4& r1, const Vector4& r2,
          const Vector4& r3, const Vector4& r4);

  Matrix4(const Matrix4& matrix) {
    memcpy(m_, matrix.m_, sizeof(m_));
  }

  const float* Pointer() const { return &(m_[0][0]);}

  float* operator[] (size_t row);
  const float* operator[] (size_t row) const;

  Matrix4& operator = (const Matrix4& matrix);

  friend Vector4 operator * (const float v[4], const Matrix4& mat);
  friend Vector4 operator * (const Vector4& v, const Matrix4& mat);
  friend Vector4 operator * (const Matrix4& mat, const Vector4& vec);
  friend Vector4 operator * (const Matrix4& mat, const float v[4]);
  friend Matrix4 operator * (const Matrix4& mat1, const Matrix4& mat2);
  friend Matrix4 operator * (const Matrix4& mat, float v);
  friend Matrix4 operator * (float v, const Matrix4& mat);

  Matrix4& operator += (const Matrix4& mat);
  Matrix4& operator -= (const Matrix4& mat);
  Matrix4 operator-();

  friend bool operator == (const Matrix4& mat1, const Matrix4& mat2);
  friend std::ostream& operator << (std::ostream& os, const Matrix4& m);

  bool IsAffine() const {
    return m_[3][0] == 0 && m_[3][1] == 0 && m_[3][2] == 0 && m_[3][3] == 1;
  }

  Matrix4 Inverse() const;
  Matrix4 InverseAffine() const;

  void Transpose();
  Matrix4 TransposeCopy() const;
  static const Matrix4 kIdentity;
  static const Matrix4 kZero;
 private:
  float m_[4][4];

  friend class Vector4;
};

}  // namespace azer
