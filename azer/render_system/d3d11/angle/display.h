#pragma once

#include "azer/render_system/d3d11/angle/precompile.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"

#include "libEGL/Display.h"

namespace azer {
class D3D11RenderSystem;

class AzerDisplay : public egl::Display {
 public:
  AzerDisplay(EGLNativeDisplayType displayId, HDC deviceContext);

  bool initialize(D3D11RenderSystem* rs);
 private:
  DISALLOW_COPY_AND_ASSIGN(AzerDisplay);
};

}  // namespace egl
