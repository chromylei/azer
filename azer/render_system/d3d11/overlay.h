#pragma once

#include "base/basictypes.h"
#include "azer/render/overlay.h"

namespace azer {

class D3D11RenderSystem;

class D3D11OverlayEffect : public OverlayEffect {
 public:
  D3D11OverlayEffect(D3D11RenderSystem* rs)
      : OverlayEffect((RenderSystem*)rs) {
  }
 protected:
  bool Init(Overlay* surface, D3D11RenderSystem* rs);
  static const char* kVertexShaderProg;
  static const char* kPixelShaderProg;

  virtual void UseTexture(Renderer*) {}
  
  friend class D3D11Overlay;
  DISALLOW_COPY_AND_ASSIGN(D3D11OverlayEffect);
};

class D3D11Overlay : public Overlay {
 public:
  D3D11Overlay(const gfx::RectF& rect, D3D11RenderSystem* rs)
      : Overlay(rect)
      , render_system_(rs) {}
  virtual ~D3D11Overlay() {}
  virtual OverlayEffect* CreateDefaultEffect();
 protected:

  D3D11RenderSystem* render_system_;
  friend class D3D11RenderSystem;
  DISALLOW_COPY_AND_ASSIGN(D3D11Overlay);
};
}  // namespace azer
