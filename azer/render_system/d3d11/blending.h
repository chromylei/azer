#pragma once

#include <d3d11.h>

#include "base/basictypes.h"
#include "azer/render/blending.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11RenderSystem;
class D3D11Renderer;

class D3D11Blending : public Blending {
 public:
  D3D11Blending(const Desc& desc, D3D11RenderSystem* rs)
        : Blending(desc)
        , blending_state_(NULL)
        , render_system_(rs) {
  }

  virtual ~D3D11Blending() {
    SAFE_RELEASE(blending_state_);
  }

  bool Init();
 protected:
  ID3D11BlendState* blending_state_;
  D3D11RenderSystem* render_system_;

  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11Blending);
};
}  // namespace azer
