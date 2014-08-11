#pragma once

namespace azer {
inline Plane::Plane()
    : normal_(0.0f, 1.0f, 0.0f)
    , d_(0.0) {
}

inline Plane::Plane(const Vector3* points)
    : point_(points[0]) {
  normal_ = CalcPlaneNormal(points);
  DCHECK(std::abs(normal_.length() - 1.0f) <  0.0001) << normal_.length();
  d_ = -normal_.dot(points[0]);
}

inline Plane::Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
  normal_ = CalcPlaneNormal(p1, p2, p3);
  DCHECK(std::abs(normal_.length() - 1.0f) <  0.0001) << normal_.length();
  d_ = -normal_.dot(p1);
}

inline Plane::Plane(const Plane& plane)
    : normal_(plane.normal_)
    , d_(plane.d_) {
  DCHECK(std::abs(normal_.length() - 1.0f) <  0.0001) << normal_.length();
}

inline Plane& Plane::operator = (const Plane& plane) {
    normal_ = plane.normal_;
    d_ = plane.d_;
    return *this;
}

inline Plane& Plane::Normalize() {
  float length = normal().length();
  normal_ /= length;
  d_ /= length;
  return *this;
}

inline float Plane::distance(const azer::Vector3& point) const {
  DCHECK(std::abs(normal_.length() - 1.0f) <  0.0001) << normal_.length();
  // ogre3d
  return normal_.dot(point) + d_;
  /*
  float dir = normal_.dot(point_ - point) > 0.0f ? 1.0f : -1.0f;
  float dist = point_.distance(point);
  */
}

inline Plane::Side Plane::GetSide(const Vector3& pt) const {
  float dist = distance(pt);
  if (dist > 0.0f) return kPositive;
  if (dist < 0.0f) return kNegative;
  else return kBothSide;
}

inline Plane::Side Plane::GetSide(const Vector3& center,
                                  const Vector3& halfsize) const {
  float dist = distance(center);
  float max_dist = std::abs(normal_.dot(halfsize));
  if (dist < -max_dist) return kNegative;
  if (dist > max_dist) return kPositive;
  return kBothSide;
}

inline Vector3 Plane::intersect(const Ray& ray) const {
  return ray.position() + ray.directional() * distance(ray);
}

inline bool Plane::belong(const Vector3& pt) const {
  return std::abs(normal_.dot(pt) + d_) < 0.0001f;
}

inline Vector4 Plane::ProjectVector(const Vector4& p) const {
  // We know plane normal is unit length, so use simple method
  Matrix4 xform;
  xform[0][0] = 1.0f - normal_.x * normal_.x;
  xform[0][1] = -normal_.x * normal_.y;
  xform[0][2] = -normal_.x * normal_.z;
  xform[1][0] = -normal_.y * normal_.x;
  xform[1][1] = 1.0f - normal_.y * normal_.y;
  xform[1][2] = -normal_.y * normal_.z;
  xform[2][0] = -normal_.z * normal_.x;
  xform[2][1] = -normal_.z * normal_.y;
  xform[2][2] = 1.0f - normal_.z * normal_.z;
  xform[3][3] = 1.0f - normal_.z * normal_.z;
  return xform * p;
}
}  // namespace math

