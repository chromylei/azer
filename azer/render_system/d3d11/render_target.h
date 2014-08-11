#pragma once

#include <d3d11.h>
#include "azer/render/render_target.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/texture.h"

namespace azer {
class D3D11RenderSystem;
class D3D11Renderer;

class D3D11RenderTarget : public RenderTarget {
 public:
  virtual ~D3D11RenderTarget() {
    SAFE_RELEASE(target_);
  }

  
  virtual void Clear(const azer::Vector4& color);
 private:
  // create by RenderSystem
  D3D11RenderTarget(const Texture::Options& opt, bool default_rt,
                    D3D11Renderer* renderer)
      : RenderTarget(opt, default_rt)
      , target_(NULL)
      , renderer_(renderer) {
  }

  bool Init(D3D11RenderSystem* );
  bool InitDefault(D3D11RenderSystem*);
  
  ID3D11RenderTargetView* target_;
  D3D11Renderer* renderer_;

  // texture_ 从 azer::RenderTarget 处继承而来
  friend class D3D11Renderer;
  friend class D3D11RenderSystem;
  DISALLOW_COPY_AND_ASSIGN(D3D11RenderTarget);
};

}  // namespace azer
