#pragma once

#include "azer/math/vector3.h"

namespace azer {

class Ray;

class Plane {
 public:
  /**
   * 从三个顶点创建一个平面
   *
   */
  Plane();
  explicit Plane(const Vector3* points);
  Plane(const Plane& plane);
  Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3);

  Plane& operator = (const Plane& plane);
  Vector3& normal() { return normal_;}
  float& d() { return d_;}

  Plane& Normalize();

  /**
   * 计算某点到平面的距离
   * Note: 如果 point 位于平面上方则返回值为正 (normal 指向上方)
   * 否则方向为负
   */
  float distance(const Vector3& point) const;

  /**
   * 射线源点沿射线方向到平面的距离
   * 如果方向与射线方向一致则距离为正值，否则为负值
   */
  float distance(const Ray& ray) const;
  
  /**
   * 计算平面与射线的焦点
   */
  Vector3 intersect(const Ray& ray) const;

  /**
   * 判断点是否属于平面
   */
  bool belong(const Vector3& pt) const;

  enum Side {
    kNegative = 1,
    kPositive,
    kBothSide,
  };

  Side GetSide(const Vector3& pt) const;
  Side GetSide(const Vector3& center, const Vector3& halfsize) const;
  
  Vector4 ProjectVector(const Vector4& v) const;
  friend std::ostream& operator << (std::ostream& o, const Plane& v);
 protected:
  // (a, b, c, d) satify a*x + b*y + c*z + d = 0
  Vector3 normal_;
  float d_;
  const Vector3 point_;
};

/**
 * class Triangle
 * 一个具有边界的 Plane
 */
class TrianglePlane : public Plane {
 public:
  explicit TrianglePlane(const azer::Vector3& p1, const azer::Vector3& p2,
                         const azer::Vector3& p3)
      : Plane(p1, p2, p3) {
    points_[0] = p1;
    points_[1] = p2;
    points_[2] = p3;
  }

  explicit TrianglePlane(const azer::Vector3* points)
      : Plane(points) {
    memcpy(points_, points, sizeof(points_));
  }

  bool IsPointIn(const Vector3& pt) const ;

  friend std::ostream& operator << (std::ostream& o, const TrianglePlane& v);
 protected:
  azer::Vector3 points_[3];
};
}  // namespace azer
