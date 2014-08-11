#include "azer/math/quaternion.h"
#include "azer/math/math.h"

namespace azer {

const float Quaternion::kEpsilon = 1e-03f;
const Quaternion Quaternion::kZero(0.0f, 0.0f, 0.0f, 0.0f);
const Quaternion Quaternion::kIdentity(1.0f, 0.0f, 0.0f, 0.0f);

Matrix4 Quaternion::ToMatrix() const {
  Matrix4 mat = Matrix4::kIdentity;
  float fTx  = x + x;
  float fTy  = y + y;
  float fTz  = z + z;
  float fTwx = fTx * w;
  float fTwy = fTy * w;
  float fTwz = fTz * w;
  float fTxx = fTx * x;
  float fTxy = fTy * x;
  float fTxz = fTz * x;
  float fTyy = fTy * y;
  float fTyz = fTz * y;
  float fTzz = fTz * z;

  mat[0][0] = 1.0f - (fTyy + fTzz);
  mat[0][1] = fTxy - fTwz;
  mat[0][2] = fTxz + fTwy;
  mat[1][0] = fTxy + fTwz;
  mat[1][1] = 1.0f - (fTxx + fTzz);
  mat[1][2] = fTyz - fTwx;
  mat[2][0] = fTxz - fTwy;
  mat[2][1] = fTyz + fTwx;
  mat[2][2] = 1.0f - (fTxx + fTyy);
  mat[3][3] = 1.0;
  return mat;
}

Quaternion Quaternion::Exp() const {
  // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
  // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
  // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

  Radians angle (sqrt(x*x+y*y+z*z));
  float fSin = sin(angle);

  Quaternion result;
  result.w = cos(angle);

  if ( std::abs(fSin) >= kEpsilon ) {
    float fCoeff = fSin/(angle.value());
    result.x = fCoeff*x;
    result.y = fCoeff*y;
    result.z = fCoeff*z;
  } else {
    result.x = x;
    result.y = y;
    result.z = z;
  }

  return result;
}

//-----------------------------------------------------------------------
Quaternion Quaternion::Log() const {
  // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
  // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
  // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

  Quaternion result;
  result.w = 0.0;

  if (std::abs(w) < 1.0) {
    Radians fAngle (acos(w));
    float fSin = sin(fAngle);
    if (std::abs(fSin) >= kEpsilon) {
      float fCoeff = fAngle.value()/fSin;
      result.x = fCoeff*x;
      result.y = fCoeff*y;
      result.z = fCoeff*z;
      return result;
    }
  }

  result.x = x;
  result.y = y;
  result.z = z;

  return result;
}

//-----------------------------------------------------------------------
Quaternion Quaternion::Slerp(float fT, const Quaternion& rkP,
                             const Quaternion& rkQ, bool shortestPath) {
  float fCos = rkP.dot(rkQ);
  Quaternion rkT;

  // Do we need to invert rotation?
  if (fCos < 0.0f && shortestPath) {
    fCos = -fCos;
    rkT = -rkQ;
  } else {
    rkT = rkQ;
  }

  if (std::abs(fCos) < 1 - kEpsilon) {
    // Standard case (slerp)
    float fSin = sqrt(1 - sqr(fCos));
    Radians fAngle = atan2(fSin, fCos);
    float fInvSin = 1.0f / fSin;
    float fCoeff0 = sin(fAngle * (1.0f - fT)) * fInvSin;
    float fCoeff1 = sin(fAngle * fT) * fInvSin;
    return rkP * fCoeff0 + rkT * fCoeff1;
  } else {
    // There are two situations:
    // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
    //    interpolation safely.
    // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
    //    are an infinite number of possibilities interpolation. but we haven't
    //    have method to fix this case, so just use linear interpolation here.
    Quaternion t = rkP * (1.0f - fT) + rkT * fT;
    // taking the complement requires renormalisation
    t.Normalize();
    return t;
  }
}

Quaternion Quaternion::SlerpExtraSpins (float fT, const Quaternion& rkP,
                                        const Quaternion& rkQ, int iExtraSpins) {
  float fCos = rkP.dot(rkQ);
  Radians fAngle (acos(fCos));

  if (std::abs(fAngle.value()) < kEpsilon)
    return rkP;

  float fSin = sin(fAngle);
  Radians fPhase ( kPI * iExtraSpins * fT );
  float fInvSin = 1.0f/fSin;
  float fCoeff0 = sin(fAngle * (1.0f-fT) - fPhase) * fInvSin;
  float fCoeff1 = sin(fAngle * fT + fPhase) * fInvSin;
  return rkP * fCoeff0 + rkQ * fCoeff1;
}

void Quaternion::Intermediate (const Quaternion& rkQ0,
                               const Quaternion& rkQ1, const Quaternion& rkQ2,
                               Quaternion& rkA, Quaternion& rkB) {
  // assert:  q0, q1, q2 are unit quaternions
  Quaternion kQ0inv = rkQ0.UnitInverse();
  Quaternion kQ1inv = rkQ1.UnitInverse();
  Quaternion rkP0 = kQ0inv*rkQ1;
  Quaternion rkP1 = kQ1inv*rkQ2;
  Quaternion kArg = (rkP0.Log()-rkP1.Log()) * 0.25;
  Quaternion kMinusArg = -kArg;

  rkA = rkQ1*kArg.Exp();
  rkB = rkQ1*kMinusArg.Exp();
}

Quaternion Quaternion::Squad(float fT, const Quaternion& rkP, const Quaternion& rkA,
                             const Quaternion& rkB, const Quaternion& rkQ,
                             bool shortestPath) {
  float fSlerpT = 2.0f*fT*(1.0f-fT);
  Quaternion kSlerpP = std::move(Slerp(fT, rkP, rkQ, shortestPath));
  Quaternion kSlerpQ = std::move(Slerp(fT, rkA, rkB));
  return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
}

Vector3 Quaternion::operator * (const Vector3& v) const {
  // nVidia SDK implementation
  Vector3 uv, uuv;
  Vector3 qvec(x, y, z);
  uv = qvec.cross(v);
  uuv = qvec.cross(uv);
  uv *= (2.0f * w);
  uuv *= 2.0f;
  
  return v + uv + uuv;
  // using matlab
  // quatmultiply(r, quatmultiply([0, v.x, v.y, nz], quatconj(r)))
  /*
  Vector3 vn(v);
  vn.Normalize();
  Quaternion q(0.0f, vn.x(), vn.y(), vn.z());
  Quaternion t = q * UnitInverse();
  Quaternion r = *this * t;
  return Vector3(r.x, r.y, r.z);
  */
}

Quaternion Quaternion::FromMatrix(const Matrix4& mat) {
  // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".
  Quaternion quat;
  float trace = mat[0][0] + mat[1][1] + mat[2][2];
  float root;

  if (trace > 0.0) {
    // |w| > 1/2, may as well choose w > 1/2
    root = sqrt(trace + 1.0f);  // 2w
    quat.w = 0.5f * root;
    root = 0.5f / root;  // 1/(4w)
    quat.x = (mat[2][1] - mat[1][2]) * root;
    quat.y = (mat[0][2] - mat[2][0]) * root;
    quat.z = (mat[1][0] - mat[0][1]) * root;
  } else {
    // |w| <= 1/2
    static size_t s_iNext[3] = { 1, 2, 0 };
    size_t i = 0;
    if (mat[1][1] > mat[0][0]) {
      i = 1;
    }
    
    if (mat[2][2] > mat[i][i]) {
      i = 2;
    }

    size_t j = s_iNext[i];
    size_t k = s_iNext[j];

    root = sqrt(mat[i][i] - mat[j][j] - mat[k][k] + 1.0f);
    float* apkQuat[3] = { &quat.x, &quat.y, &quat.z };
    *apkQuat[i] = 0.5f * root;
    root = 0.5f / root;
    quat.w = (mat[k][j] - mat[j][k]) * root;
    *apkQuat[j] = (mat[j][i] + mat[i][j]) * root;
    *apkQuat[k] = (mat[k][i] + mat[i][k]) * root;
  }

  return quat;
}

Quaternion Quaternion::FromAxis(const Vector3& xaxis, const Vector3& yaxis,
                                const Vector3& zaxis) {
  Matrix4 mat = Matrix4::kIdentity;

  mat[0][0] = xaxis.x;
  mat[1][0] = xaxis.y;
  mat[2][0] = xaxis.z;

  mat[0][1] = yaxis.x;
  mat[1][1] = yaxis.y;
  mat[2][1] = yaxis.z;

  mat[0][2] = zaxis.x;
  mat[1][2] = zaxis.y;
  mat[2][2] = zaxis.z;

  return FromMatrix(mat);
}

}  // namespace azer
