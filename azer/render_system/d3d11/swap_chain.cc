#include "azer/render_system/d3d11/swap_chain.h"

#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/render_target.h"
#include "azer/render_system/d3d11/depth_buffer.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/ui/window/window_host.h"

namespace azer {
D3D11SwapChain::D3D11SwapChain(D3D11RenderSystem* rs)
    : swap_chain_(NULL)
    , render_system_(rs) {
}

D3D11SwapChain::~D3D11SwapChain() {
  SAFE_RELEASE(swap_chain_);
}

bool D3D11SwapChain::Init(int width, int height)  {
  DCHECK(NULL == swap_chain_);
  return reset(width, height);
}

Renderer* D3D11SwapChain::CreateDefault(int width, int height) {
  ID3D11DeviceContext* d3d_context = render_system_->GetContext();
  azer::Texture::Options o;
  o.width = width;
  o.height = height;
  o.target = Texture::kRenderTarget;
  std::unique_ptr<D3D11Renderer> renderer(
      new D3D11Renderer(d3d_context, render_system_));
  
  std::unique_ptr<D3D11RenderTarget> target(
          new D3D11RenderTarget(o, true, renderer.get()));
  if (!target->InitDefault(o, this, render_system_)) {
    return false;
  }

  Texture::Options depth_opt;
  depth_opt.width = o.width;
  depth_opt.height = o.height;
  depth_opt.format = kDepth24Stencil8;
  depth_opt.target = Texture::kDepthStencil;
  std::unique_ptr<D3D11DepthBuffer> depth(new D3D11DepthBuffer(depth_opt,
                                                               renderer.get()));
  if (!depth->Init(render_system_)) {
    return false;
  }

  if (!renderer->InitDefault(o, target.get(), depth.get())) {
    return false;
  }

  target.release();
  depth.release();
  return renderer.release();
}

bool D3D11SwapChain::resize(int width, int height) {
  CHECK(false);
  return true;
}

bool D3D11SwapChain::Present() {
  DCHECK(NULL != swap_chain_);
  HRESULT hr = swap_chain_->Present(0, 0);
  if (FAILED(hr)) {
    if (hr == DXGI_ERROR_DEVICE_RESET) {
      LOG(ERROR) << "Present error: DXGI_ERROR_DEVICE_RESET";
    } else if (hr == DXGI_ERROR_DEVICE_REMOVED) {
      LOG(ERROR) << "Present error: DXGI_ERROR_DEVICE_REMOVED";
    } else if (hr == DXGI_STATUS_OCCLUDED) {
      LOG(ERROR) << "Present error: DXGI_STATUS_OCCLUDED";
    } else {
      CHECK(false) << "unknown";
    }
    return false;
  }

  return true;
}

bool D3D11SwapChain::reset(int width, int height) {
  WindowHost* winhost = render_system_->GetWindowHost();
  ID3D11Device* d3d_device = render_system_->GetDevice();
  HRESULT hr;

  //Describe our SwapChain
  DXGI_MODE_DESC bufferDesc;
  ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
  bufferDesc.Width = width;
  bufferDesc.Height = height;
  bufferDesc.RefreshRate.Numerator = 60;
  bufferDesc.RefreshRate.Denominator = 1;
  bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  DXGI_SWAP_CHAIN_DESC swapChainDesc; 
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
  swapChainDesc.BufferDesc = bufferDesc;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 1;
  swapChainDesc.OutputWindow = (HWND)winhost->Handle();
  swapChainDesc.Windowed = !winhost->GetMetrics().fullscreen;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  IDXGIFactory* factory = render_system_->GetDxgiFactory();
  hr = factory->CreateSwapChain(d3d_device, &swapChainDesc, &swap_chain_);
  HRESULT_HANDLE(hr, ERROR, "Failed to create D3D11 and Swapchain ");
  renderer_.reset(CreateDefault(width, height));
  if (renderer_.get() == NULL) {
    return false;
  }

  return true;
}

}  // namespace azer
