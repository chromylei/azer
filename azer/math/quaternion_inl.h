
namespace azer {

inline Quaternion::Quaternion(const Matrix4& mat) {
  *this = FromMatrix(mat);
}

inline Quaternion::Quaternion(const Vector3& axis, const Radians rad) {
  float half_sin = sin(rad / 2.0f);
  float half_cos = cos(rad / 2.0f);
  w = half_cos;
  x = half_sin * axis.x;
  y = half_sin * axis.y;
  z = half_sin * axis.z;
}

inline Quaternion::Quaternion(const Vector3& axis, const Degree angle) {
  float half_sin = sin(angle / 2.0f);
  float half_cos = cos(angle / 2.0f);
  w = half_cos;
  x = half_sin * axis.x;
  y = half_sin * axis.y;
  z = half_sin * axis.z;
}

inline void Quaternion::ToAxis(Vector3* axis) const {
  Matrix4 rot = std::move(ToMatrix());
  for (size_t col = 0; col < 3; ++col) {
    axis[col] = Vector3(rot[0][col], rot[1][col], rot[2][col]);
  }
}

inline void Quaternion::ToAxis(Vector3* xaxis, Vector3* yaxis,
                               Vector3* zaxis) const {
  Matrix4 rot = std::move(ToMatrix());
  *xaxis = Vector3(rot[0][0], rot[1][0], rot[2][0]);
  *yaxis = Vector3(rot[0][1], rot[1][1], rot[2][1]);
  *zaxis = Vector3(rot[0][2], rot[1][2], rot[2][2]);
}

inline Vector3 Quaternion::xaxis() const {
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwy = fTy * w;
  float fTwz = fTz * w;
  float fTxy = fTy * x;
  float fTxz = fTz * x;
  float fTyy = fTy * y;
  float fTzz = fTz * z;

  return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}

inline Vector3 Quaternion::yaxis() const {
  float fTx  = 2.0f * x;
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwx = fTx * w;
  float fTwz = fTz * w;
  float fTxx = fTx * x;
  float fTxy = fTy * x;
  float fTyz = fTz * y;
  float fTzz = fTz * z;

  return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
}

inline Vector3 Quaternion::zaxis() const {
  float fTx  = 2.0f * x;
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwx = fTx * w;
  float fTwy = fTy * w;
  float fTxx = fTx * x;
  float fTxz = fTz * x;
  float fTyy = fTy * y;
  float fTyz = fTz * y;

  return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
}

inline void Quaternion::ToAxisAngle(Vector3* axis, Degree* degree) const {
  float len  = length();
  if (len > 0.0f) {
    *degree = Degree(Radians(2.0 * acos(w)));
    float invlen = 1 / len;
    *axis = Vector3(x * invlen, y * invlen, z * invlen);
  } else {
    *axis = Vector3(1.0f, 0.0f, 0.0f);
    *degree = Degree(0.0f);
  }
}

inline void Quaternion::ToAxisAngle(Vector3* axis, Radians* rad) const {
  float len  = length();
  if (len > 0.0f) {
    *rad = Radians(2.0 * acos(w));
    float invlen = 1 / len;
    *axis = Vector3(x * invlen, y * invlen, z * invlen);
  } else {
    *axis = Vector3(1.0f, 0.0f, 0.0f);
    *rad = Radians(0.0f);
  }
}

inline Quaternion Quaternion::operator + (const Quaternion& q) const {
  return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

inline Quaternion Quaternion::operator - (const Quaternion& q) const {
  return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

inline Quaternion Quaternion::operator - () const {
  return Quaternion(-w, -x, -y, -z);
}

inline Quaternion Quaternion::operator * (float scale) const {
  return Quaternion(w * scale, x * scale, y * scale, z * scale);
}

inline Quaternion operator * (const Quaternion &q, const Quaternion& r) {
  float w = r.w*q.w - r.x*q.x - r.y*q.y - r.z*q.z;
  float x = r.w*q.x + r.x*q.w - r.y*q.z + r.z*q.y;
  float y = r.w*q.y + r.x*q.z + r.y*q.w - r.z*q.x;
  float z = r.w*q.z - r.x*q.y + r.y*q.x + r.z*q.w;
  return Quaternion(w, x, y, z);
}

inline float Quaternion::length() const {
  float len = w*w + x*x + y*y + z*z;
  return sqrt(len);
}

inline Quaternion& Quaternion::Normalize() {
  float factor = 1.0f / length();
  *this = *this * factor;
  return *this;
}

inline Quaternion Quaternion::NormalizeCopy() const {
  float factor = 1.0f / length();
  return *this * factor;
}

inline float Quaternion::dot (const Quaternion& q) const {
  return w*q.w + x*q.x + y*q.y + z*q.z;
}

inline Quaternion Quaternion::Inverse() const {
  float fNorm = w*w + x*x + y*y + z*z;
  if (fNorm > 0.0) {
    float fInvNorm = 1.0f / fNorm;
    return Quaternion(w*fInvNorm, -x*fInvNorm, -y*fInvNorm, -z*fInvNorm);
  } else {
    // return an invalid result to flag the error
    return kZero;
  }
}

inline Quaternion Quaternion::UnitInverse() const {
  // assert:  'this' is unit length
  DCHECK_FLOAT_EQ(length(), 1.0);
  return Quaternion(w,-x,-y,-z);
}


inline Vector4 Quaternion::AxisX() const {
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwy = fTy * w;
  float fTwz = fTz * w;
  float fTxy = fTy * x;
  float fTxz = fTz * x;
  float fTyy = fTy * y;
  float fTzz = fTz * z;

  return Vector4(1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy, 0.0f);
}

inline Vector4 Quaternion::AxisY() const {
  float fTx  = 2.0f * x;
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwx = fTx * w;
  float fTwz = fTz * w;
  float fTxx = fTx * x;
  float fTxy = fTy * x;
  float fTyz = fTz * y;
  float fTzz = fTz * z;

  return Vector4(fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx, 1.0f);
}

inline Vector4 Quaternion::AxisZ() const {
  float fTx  = 2.0f * x;
  float fTy  = 2.0f * y;
  float fTz  = 2.0f * z;
  float fTwx = fTx * w;
  float fTwy = fTy * w;
  float fTxx = fTx * x;
  float fTxz = fTz * x;
  float fTyy = fTy * y;
  float fTyz = fTz * y;

  return Vector4(fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy), 1.0f);
}

inline Quaternion Quaternion::nlerp(float fT, const Quaternion& rkP,
                                    const Quaternion& rkQ, bool shortestPath) {
  Quaternion result;
  float fCos = rkP.dot(rkQ);
  if (fCos < 0.0f && shortestPath) {
    result = rkP + ((-rkQ) - rkP) * fT;
  } else {
    result = rkP + (rkQ - rkP) * fT;
  }

  result.Normalize();
  return result;
}

inline bool operator == (const Quaternion& q1, const Quaternion& q2) {
  return (std::abs(q1.x - q2.x) < kFloatMargin)
      && (std::abs(q1.y - q2.y) < kFloatMargin)
      && (std::abs(q1.z - q2.z) < kFloatMargin)
      && (std::abs(q1.w - q2.w) < kFloatMargin);
}

inline bool Quaternion::isnan() const {
  return azer::isnan(w) && azer::isnan(x) && azer::isnan(y) && azer::isnan(z);
}

inline float Quaternion::operator[] (const size_t i) const {
  return (&w)[i];
}

inline float& Quaternion::operator[] (const size_t i) {
  return (&w)[i];
}

inline float* Quaternion::ptr() {
  return &w;
}

inline const float* Quaternion::ptr() const {
  return &w;
}

inline Quaternion Quaternion::FromRotate(const Vector3& axis, Radians rad) {
  return Quaternion(axis, rad);
}

inline std::ostream& operator << (std::ostream& os, const Quaternion& q) {
  os << std::setprecision (8)
     << "quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
  return os;
}

}  // namespace azer
