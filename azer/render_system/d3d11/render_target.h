#pragma once

#include <d3d11.h>
#include "azer/render/render_target.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/swap_chain.h"

namespace azer {
class D3D11RenderSystem;
class D3D11Renderer;

class D3D11RenderTarget : public RenderTarget {
 public:
  // create by RenderSystem
  D3D11RenderTarget(const Texture::Options& opt, bool default_rt,
                    D3D11Renderer* renderer)
      : RenderTarget(opt, default_rt)
      , target_(NULL)
      , renderer_(renderer) {
  }
  
  virtual ~D3D11RenderTarget() {
    SAFE_RELEASE(target_);
  }

  
  virtual void Clear(const azer::Vector4& color);

  bool Init(D3D11RenderSystem* rs);
  bool InitDefault(const Texture::Options& opt, D3D11SwapChain*);
  
  ID3D11RenderTargetView* GetD3D11RenderTargetView() { return target_;}
 private:
  ID3D11RenderTargetView* target_;
  D3D11Renderer* renderer_;

  DISALLOW_COPY_AND_ASSIGN(D3D11RenderTarget);
};

}  // namespace azer
