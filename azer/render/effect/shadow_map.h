#pragma once

#include "azer/base/render_export.h"
#include "azer/render/light.h"
#include "azer/render/depth_buffer.h"
#include "azer/render/effect.h"
#include "azer/render/render_target.h"

namespace azer {

class RenderSystem;
class RenderQueue;

class AZER_EXPORT ShadowMapEffect : public Effect {
 public:
 private:
  DISALLOW_COPY_AND_ASSIGN(ShadowMapEffect);
};

class AZER_EXPORT ShadowMap {
 public:
  void SetLight(const Light& light);
 private:
  DepthBufferPtr depth_;
  RenderTargetPtr target_;
  DISALLOW_COPY_AND_ASSIGN(ShadowMap);
};
}  // namespace azer
