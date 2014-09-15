#pragma once

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"
#include "azer/render/bounding_volumn.h"

namespace azer {

class Camera;
class RenderSystem;
class VertexData;
class Mesh;

class AZER_EXPORT AxisAlignedBox : public BoundingVolumn {
 public:
  AxisAlignedBox();
  AxisAlignedBox(const Vector4& minimum, const Vector4& maximum);
  AxisAlignedBox(const Vector3& minimum, const Vector3& maximum);
  AxisAlignedBox(const AxisAlignedBox& box);
  static AxisAlignedBox CalcForVertexData(const VertexData& vdata);

  AxisAlignedBox& operator = (const AxisAlignedBox& box);

  /**
   * 在物体变换过程中（包括缩放，位移，旋转等）
   * 想要使得 AABB 能够继续包围物体必须同时对 AABB 进行变换
   */
  AxisAlignedBox TransformCopy(const Matrix4& trans);
  void merge(const AxisAlignedBox& aabox);

  const Vector4& minimum() const { return minimum_;}
  const Vector4& maximum() const { return maximum_;};
  void set_min(const Vector4& m) { minimum_ = m;}
  void set_max(const Vector4& m) { maximum_ = m;}

  bool intersect(const Ray& ray) const;
  bool intersect(const AxisAlignedBox& box) const;
  bool inside(const Vector3& pos) const;
  bool IsNull() const { return is_null_;}

  // from Bounding Volumn
  virtual VisibleState IsVisible(const Frustrum& frustrum,
                                 const Matrix4& world) OVERRIDE;
  virtual void Render(Renderer* renderer, const Matrix4& world,
                      const Camera& camera) OVERRIDE;
  friend AZER_EXPORT std::ostream& operator << (std::ostream& o,
                                                const AxisAlignedBox& v);
 private:
  Vector4 minimum_;
  Vector4 maximum_;
  bool is_null_;
};

inline AxisAlignedBox::AxisAlignedBox()
    : is_null_(true)  {
}

inline AxisAlignedBox::AxisAlignedBox(
    const AxisAlignedBox& box) {
  *this = box;
}

inline AxisAlignedBox& AxisAlignedBox::operator = (
    const AxisAlignedBox& box) {
  this->minimum_ = box.minimum_;
  this->maximum_ = box.maximum_;
  this->is_null_ = box.is_null_;
  return *this;
}

inline AxisAlignedBox::AxisAlignedBox(const Vector3& minimum,
                                                      const Vector3& maximum)
    : minimum_(minimum, 1.0f)
    , maximum_(maximum, 1.0f)
    , is_null_(false) {
  DCHECK(minimum_.x <= maximum_.x &&
         minimum_.y <= maximum_.y &&
         minimum_.z <= maximum_.z);
}

inline AxisAlignedBox::AxisAlignedBox(const Vector4& minimum,
                                                      const Vector4& maximum)
    : minimum_(minimum)
    , maximum_(maximum)
    , is_null_(false) {
  DCHECK(minimum_.x <= maximum_.x &&
         minimum_.y <= maximum_.y &&
         minimum_.z <= maximum_.z);
}

inline void AxisAlignedBox::merge(const AxisAlignedBox& aabbox) {
  minimum_.x = std::min(aabbox.minimum().x, minimum_.x);
  minimum_.y = std::min(aabbox.minimum().y, minimum_.y);
  minimum_.z = std::min(aabbox.minimum().z, minimum_.z);

  maximum_.x = std::max(aabbox.maximum().x, maximum_.x);
  maximum_.y = std::max(aabbox.maximum().y, maximum_.y);
  maximum_.z = std::max(aabbox.maximum().z, maximum_.z);
  is_null_ = false;
}

inline bool AxisAlignedBox::inside(const Vector3& pos) const {
  return minimum_.x <= pos.x && maximum_.x >= pos.x  &&
      minimum_.y <= pos.y && maximum_.y >= pos.y  &&
      minimum_.z <= pos.z && maximum_.z >= pos.z;
}

inline bool AxisAlignedBox::intersect(const AxisAlignedBox& box)
    const {
  return inside(box.minimum_) || inside(box.maximum_);
}
}  // namespace azer
