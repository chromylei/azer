#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

#include <string>

#include "base/logging.h"
#include "azer/render/swap_chain.h"

namespace azer {
class D3D11RenderSystem;
class D3D11Renderer;

class D3D11SwapChain : public SwapChain {
 public:
  D3D11SwapChain(D3D11RenderSystem* rs, WindowHost* host);
  virtual ~D3D11SwapChain();

  virtual bool Init() OVERRIDE;
  virtual bool reset(int width, int height) OVERRIDE;
  virtual bool resize(int width, int height) OVERRIDE;
  virtual void Present() OVERRIDE;

  ID3D11Device* GetDevice();
  ID3D11DeviceContext* GetContext();
  IDXGISwapChain* GetD3D11SwapChain();

  D3D_FEATURE_LEVEL feature_level() const { return feature_level_; }
 private:
  IDXGISwapChain* swap_chain_;
  ID3D11Device* d3d_device_;
  ID3D11DeviceContext* d3d_context_;
  D3D_FEATURE_LEVEL feature_level_;
  D3D11RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(D3D11SwapChain);
};

inline ID3D11DeviceContext* D3D11SwapChain::GetContext() {
  DCHECK(NULL != d3d_context_);
  return d3d_context_;
}
inline ID3D11Device* D3D11SwapChain::GetDevice() {
  DCHECK(NULL != d3d_device_);
  return d3d_device_;
}

inline IDXGISwapChain* D3D11SwapChain::GetD3D11SwapChain() {
  DCHECK(NULL != swap_chain_);
  return swap_chain_;
}
}  // namespace azer
