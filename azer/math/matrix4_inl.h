
namespace azer {

inline Matrix4::Matrix4(float v[16]) {
  m_[0][0] = v[0]; m_[0][1] = v[1]; m_[0][2] = v[2]; m_[0][3] = v[3];
  m_[1][0] = v[4]; m_[1][1] = v[5]; m_[1][2] = v[6]; m_[1][3] = v[7];
  m_[2][0] = v[8]; m_[2][1] = v[9]; m_[2][2] = v[10]; m_[2][3] = v[11];
  m_[3][0] = v[12]; m_[3][1] = v[13]; m_[3][2] = v[14]; m_[3][3] = v[15];
}

inline Matrix4::Matrix4(const float v[16]) {
  m_[0][0] = v[0]; m_[0][1] = v[1]; m_[0][2] = v[2]; m_[0][3] = v[3];
  m_[1][0] = v[4]; m_[1][1] = v[5]; m_[1][2] = v[6]; m_[1][3] = v[7];
  m_[2][0] = v[8]; m_[2][1] = v[9]; m_[2][2] = v[10]; m_[2][3] = v[11];
  m_[3][0] = v[12]; m_[3][1] = v[13]; m_[3][2] = v[14]; m_[3][3] = v[15];
}

inline Matrix4::Matrix4(float _00, float _01, float _02, float _03,
                        float _10, float _11, float _12, float _13,
                        float _20, float _21, float _22, float _23,
                        float _30, float _31, float _32, float _33) {
  m_[0][0] = _00; m_[0][1] = _01; m_[0][2] = _02; m_[0][3] = _03;
  m_[1][0] = _10; m_[1][1] = _11; m_[1][2] = _12; m_[1][3] = _13;
  m_[2][0] = _20; m_[2][1] = _21; m_[2][2] = _22; m_[2][3] = _23;
  m_[3][0] = _30; m_[3][1] = _31; m_[3][2] = _32; m_[3][3] = _33;
}

inline Matrix4::Matrix4(const Vector4& row1, const Vector4& row2,
                        const Vector4& row3, const Vector4& row4) {
  m_[0][0] = row1.x; m_[0][1] = row1.y; m_[0][2] = row1.z;
  m_[0][3] = row1.w;

  m_[1][0] = row2.x; m_[1][1] = row2.y; m_[1][2] = row2.z;
  m_[1][3] = row2.w;

  m_[2][0] = row3.x; m_[2][1] = row3.y; m_[2][2] = row3.z;
  m_[2][3] = row3.w;
  
  m_[3][0] = row4.x; m_[3][1] = row4.y; m_[3][2] = row4.z;
  m_[3][3] = row4.w;
}

inline float* Matrix4::operator[] (size_t row) {
  CHECK_LT(row, 4);
  return m_[row];
}

inline const float* Matrix4::operator[] (size_t row) const {
  CHECK_LT(row, 4);
  return m_[row];
}

inline Vector4 operator* (const Matrix4& mat, const float v[4]) {
  return Vector4(
      mat[0][0] * v[0] + mat[0][1] * v[1] + mat[0][2] * v[2] + mat[0][3] * v[3],
      mat[1][0] * v[0] + mat[1][1] * v[1] + mat[1][2] * v[2] + mat[1][3] * v[3],
      mat[2][0] * v[0] + mat[2][1] * v[1] + mat[2][2] * v[2] + mat[2][3] * v[3],
      mat[3][0] * v[0] + mat[3][1] * v[1] + mat[3][2] * v[2] + mat[3][3] * v[3]
                 );
}

inline Vector4 operator * (const Matrix4& mat, const Vector4& vec) {
  return mat * vec.Pointer();
}

inline Matrix4 operator * (const Matrix4& mat1, const Matrix4& mat2) {
  Vector4 row1 = (mat1.Pointer())      * mat2;
  Vector4 row2 = (mat1.Pointer() + 4)  * mat2;
  Vector4 row3 = (mat1.Pointer() + 8)  * mat2;
  Vector4 row4 = (mat1.Pointer() + 12) * mat2;
  return Matrix4(row1, row2, row3, row4);
}

inline bool operator == (const Matrix4& mat1, const Matrix4& mat2) {
  const float * val1 = mat1.Pointer();
  const float * val2 = mat2.Pointer();
  for (int i = 0; i < 16; ++i) {
    if (std::abs((*val1++) - (*val2++)) > kFloatMargin) {
      return false;
    }
  }

  return true;
}

inline std::ostream& operator << (std::ostream& os, const Matrix4& m) {
  os << "mat {" << std::endl
     << m[0][0] << ",\t" << m[0][1] << ",\t" << m[0][2] << ",\t"
     << m[0][3] << std::endl
     << m[1][0] << ",\t" << m[1][1] << ",\t" << m[1][2] << ",\t"
     << m[1][3] << std::endl
     << m[2][0] << ",\t" << m[2][1] << ",\t" << m[2][2] << ",\t"
     << m[2][3] << std::endl
     << m[3][0] << ",\t" << m[3][1] << ",\t" << m[3][2] << ",\t"
     << m[3][3] << std::endl
     << "}";
  return os;
}

inline Matrix4& Matrix4::operator = (const Matrix4& matrix) {
  memcpy(m_, matrix.m_, sizeof(m_));
  return *this;
}

inline void Matrix4::Transpose() {
  using std::swap;
  swap(m_[0][1], m_[1][0]); swap(m_[0][2], m_[2][0]); swap(m_[0][3], m_[3][0]);
  swap(m_[1][1], m_[1][1]); swap(m_[1][2], m_[2][1]); swap(m_[1][3], m_[3][1]);
  swap(m_[2][1], m_[1][2]); swap(m_[2][2], m_[2][2]); swap(m_[2][3], m_[3][2]);
  swap(m_[3][1], m_[1][3]); swap(m_[3][2], m_[2][3]); swap(m_[3][3], m_[3][3]);
}

inline Matrix4 Matrix4::TransposeCopy() const {
  return Matrix4(m_[0][0], m_[1][0], m_[2][0], m_[3][0],
                 m_[0][1], m_[1][1], m_[2][1], m_[3][1],
                 m_[0][2], m_[1][2], m_[2][2], m_[3][2],
                 m_[0][3], m_[1][3], m_[2][3], m_[3][3]);
}

inline Matrix4 Matrix4::operator-() {
  Matrix4 mat;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      mat[i][j] = - m_[i][j];
    }
  }
  return mat;
}

inline Matrix4& Matrix4::operator += (const Matrix4& mat) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      m_[i][j] += mat[i][j];
    }
  }
  return *this;
}

inline Matrix4& Matrix4::operator -= (const Matrix4& mat) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      m_[i][j] -= mat[i][j];
    }
  }
  return *this;
}

inline Matrix4 operator * (const Matrix4& mat, float v) {
  Matrix4 m;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      m[i][j] = v * mat[i][j];
    }
  }
  return m;
}

inline Matrix4 operator * (float v, const Matrix4& mat) {
  return mat * v;
}
}  // namespace azer
