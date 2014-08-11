#pragma once

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"

namespace azer {

class RenderSystem;
class Bone;

class AZER_EXPORT Skeleton {
 public:
  Skeleton() : root_(NULL) {}
  ~Skeleton();

  /**
   * 绘制骨骼
   */
  void RenderSkeleton(RenderSystem* rs);

  /**
   * 在两个骨骼上做插值
   * whole: 代表动画完成的所有时间
   */
  friend void Interpolation(Skeleton* s1, Skeleton* s2, float ft, float whole);
 private:
  Bone* root_;

  DISALLOW_COPY_AND_ASSIGN(Skeleton);
};
}  // namespace azer
