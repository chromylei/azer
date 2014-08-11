#pragma once

#include "azer/base/render_export.h"

namespace azer {

class Effect;
class Renderer;
class RenderSystem;

class AZER_EXPORT Renderable {
 public:
  virtual ~Renderable() {};

  // render
  virtual void PrepareEffect(Renderer* renderer, Effect* effect) = 0;
  virtual void Render(Renderer* renderer) = 0;

  
  virtual void RenderBoundingVolumn(Renderer* renderer, const Matrix4& world,
                                    const Camera& camera) = 0;
};

typedef std::shared_ptr<Renderable> RenderablePtr;
}  // namespace azer
