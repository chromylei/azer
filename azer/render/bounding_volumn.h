#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/base/render_export.h"

namespace azer {

class Camera;
class Renderer;
class Matrix4;

class Frustrum;
class AZER_EXPORT BoundingVolumn {
 public:
  enum Type {
    kNoBoundingVolumn,
    kSphere,
    kAxisAlignedBox,
    kOrientedBox,
  };

  virtual bool IsVisible(const Frustrum& frustrum, const Matrix4& world) = 0;
  virtual void Render(Renderer* renderer, const Matrix4& world,
                      const Camera& camera) = 0;
};

typedef std::shared_ptr<BoundingVolumn> BoundingVolumnPtr;
}  // namespace azer
