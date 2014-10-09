#pragma once

#include "azer/angle/precompile.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"

namespace azer {

class D3D11RenderSystem;
class AzerRenderer : public rx::Renderer11 {
 public:
  AzerRenderer(egl::Display *display, HDC hDc, D3D11RenderSystem* rs);

  EGLint initialize();
 private:
  DISALLOW_COPY_AND_ASSIGN(AzerRenderer);
};

}  // namespace azer
