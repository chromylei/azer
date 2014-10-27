#include "azer/render_system/d3d11/render_target.h"

#include <string>

#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/swap_chain.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {

bool D3D11RenderTarget::InitDefault(const Texture::Options& opt, D3D11SwapChain* sc) {
  DCHECK(target_ == NULL);
  DCHECK(default_render_target_);
  HRESULT hr;

  IDXGISwapChain* swap_chain = sc->GetD3D11SwapChain();
  ID3D11Device* d3d_device = sc->GetDevice();
  ID3D11Texture2D* texture_buffer;
  hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture_buffer);
  HRESULT_HANDLE(hr, ERROR, "SwapChain::GetBuffer failed ");
  texture_.reset(new D3D11Texture2D(opt, (D3D11RenderSystem*)renderer_->GetRenderSystem()));
  ((D3D11Texture2D*)texture_.get())->Attach(texture_buffer);
  hr = d3d_device->CreateRenderTargetView(
      ((D3D11Texture2D*)texture_.get())->resource_, NULL, &target_);
  HRESULT_HANDLE(hr, ERROR, "CreateRenderTargetView failed ");
  
  return true;
}

void D3D11RenderTarget::Clear(const azer::Vector4& color) {
  DCHECK(NULL != target_);
  DCHECK(NULL != renderer_);
  ID3D11DeviceContext* d3d_context = renderer_->GetContext();
  d3d_context->ClearRenderTargetView(
      target_, D3DXCOLOR(color.x, color.y, color.z, color.w));
}

bool D3D11RenderTarget::Init(D3D11RenderSystem* rs) {
  DCHECK(!default_render_target_);
  DCHECK(target_ == NULL);

  ID3D11Device* d3d_device = rs->GetDevice();
  HRESULT hr;
  DCHECK(texture_.get() == NULL);
  D3D11Texture2D* tex = new D3D11Texture2D(options_, rs);
  texture_.reset(tex);
  if (!tex->Init(NULL, 1)) {
    return false;
  }

  ID3D11Resource* resource = ((D3D11Texture2D*)texture_.get())->resource_;
  DCHECK(TranslateBindTarget(options_.target) & D3D11_BIND_RENDER_TARGET);

  hr = d3d_device->CreateRenderTargetView(resource, NULL, &target_);
  HRESULT_HANDLE(hr, ERROR, "CreateRenderTargetView failed ");
  return true;
}
}  // namespace azer
