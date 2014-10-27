#pragma once

#include <d3d11.h>
#include <stack>

#include "base/logging.h"
#include "azer/render/depth_buffer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11RenderSystem;
class D3D11Renderer;

class D3D11DepthBuffer : public DepthBuffer {
 public:
   // create by rendersystem
  D3D11DepthBuffer(const Texture::Options& opt, D3D11Renderer* renderer)
      : DepthBuffer(opt)
      , stencil_ref_value_(0)
      , target_(NULL)
      , renderer_(renderer) {
  }

  virtual ~D3D11DepthBuffer() {
    SAFE_RELEASE(target_);
  }

  
  virtual void Enable(bool enable);
  virtual bool IsEnabled();

  virtual void SetDepthCompareFunc(CompareFunc::Type func);

  virtual void EnableWrite(bool) {CHECK(false);}
  virtual void SetBias(float val) {CHECK(false);}

  virtual void PushState();
  virtual void PopState();

  bool Init(D3D11RenderSystem* rs);
  void Clear(D3D11Renderer*, ClearFlag flag = kClearAll, float depth_val = 1.0,
             int stencil_val = 0);

  ID3D11DepthStencilView* GetD3D11DepthStencilView() { return target_;}
 private:
  void UpdateState();
  bool InitDepthAndStencilState(D3D11RenderSystem* rs);

  std::stack<D3D11_DEPTH_STENCIL_DESC> state_stack_;

  int32 stencil_ref_value_;
  ID3D11DepthStencilView* target_;
  D3D11Renderer* renderer_;
  D3D11_DEPTH_STENCIL_DESC desc_;
  friend class D3D11Renderer;
  friend class D3D11RenderSystem;
  DISALLOW_COPY_AND_ASSIGN(D3D11DepthBuffer);
};


}  // namespace azer
