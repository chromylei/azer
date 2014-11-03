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
  D3D11SwapChain(D3D11RenderSystem* rs);
  virtual ~D3D11SwapChain();

  bool Init(int width, int height);
  virtual bool reset(int width, int height) OVERRIDE;
  virtual bool resize(int width, int height) OVERRIDE;
  virtual bool Present() OVERRIDE;
  
  IDXGISwapChain* GetD3D11SwapChain();
 private:
  Renderer* CreateDefault(int width, int height);
  IDXGISwapChain* swap_chain_;
  D3D11RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(D3D11SwapChain);
};

inline IDXGISwapChain* D3D11SwapChain::GetD3D11SwapChain() {
  DCHECK(NULL != swap_chain_);
  return swap_chain_;
}
}  // namespace azer
