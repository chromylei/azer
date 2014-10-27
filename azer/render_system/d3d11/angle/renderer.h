#pragma once

#include "azer/render_system/d3d11/angle/precompile.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"

namespace azer {

class D3D11RenderSystem;
class AzerRenderer : public rx::Renderer11 {
 public:
  AzerRenderer(egl::Display *display, HDC hDc, D3D11RenderSystem* rs);

  EGLint initialize();

  virtual rx::SwapChain *createSwapChain(HWND window, HANDLE shareHandle,
                                         GLenum backBufferFormat,
                                         GLenum depthBufferFormat);
  D3D11RenderSystem* GetRenderSystem() { return render_system_;}
 private:
  D3D11RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(AzerRenderer);
};

}  // namespace azer
