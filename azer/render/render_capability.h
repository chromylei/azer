#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"

namespace azer {
class AZER_EXPORT RenderSystemCapability {
 public:
  bool HarewareTransformAndLightingSupported() const {
    return hw_transform_light_;
  }
 private:
  // Device can support transformation and lighting in hardware.
  bool hw_transform_light_;

  int32 max_simultaneous_textures_;
  friend class D3D9RenderSystem;
  friend class D3D11RenderSystem;
};
}  // namespace azer
