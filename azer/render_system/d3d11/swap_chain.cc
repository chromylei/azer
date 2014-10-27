#include "azer/render_system/d3d11/swap_chain.h"

#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/render_target.h"
#include "azer/render_system/d3d11/depth_buffer.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/ui/window/window_host.h"

namespace azer {
D3D11SwapChain::D3D11SwapChain(D3D11RenderSystem* rs, WindowHost* host)
    : SwapChain(host)
    , swap_chain_(NULL)
    , d3d_device_(NULL)
    , d3d_context_(NULL)
    , render_system_(rs) {
}

D3D11SwapChain::~D3D11SwapChain() {
  SAFE_RELEASE(swap_chain_);
  SAFE_RELEASE(d3d_device_);
}

bool D3D11SwapChain::Init()  {
  DCHECK(NULL == swap_chain_);
  DCHECK(NULL == d3d_device_);
  HRESULT hr;

  //Describe our Buffer
  DXGI_MODE_DESC bufferDesc;
  ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
  bufferDesc.Width = win_host_->GetMetrics().width;
  bufferDesc.Height = win_host_->GetMetrics().height;
  bufferDesc.RefreshRate.Numerator = 60;
  bufferDesc.RefreshRate.Denominator = 1;
  bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  
  //Describe our SwapChain
  DXGI_SWAP_CHAIN_DESC swapChainDesc; 
  
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapChainDesc.BufferDesc = bufferDesc;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 1;
  swapChainDesc.OutputWindow = (HWND)win_host_->Handle();
  swapChainDesc.Windowed = !win_host_->GetMetrics().fullscreen;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  // Create our SwapChain
  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
                                     NULL, NULL,
                                     featureLevels,
                                     arraysize(featureLevels),
                                     D3D11_SDK_VERSION, &swapChainDesc,
                                     &swap_chain_, &d3d_device_,
                                     &feature_level_,
                                     &d3d_context_);
  HRESULT_HANDLE(hr, ERROR, "Failed to create D3D11 and Swapchain ");

  azer::Texture::Options o;
  o.width = win_host_->GetMetrics().width;
  o.height = win_host_->GetMetrics().height;
  o.target = Texture::kRenderTarget;
  std::unique_ptr<D3D11Renderer> renderer(
          new D3D11Renderer(d3d_context_, render_system_));
  
  std::unique_ptr<D3D11RenderTarget> target(
          new D3D11RenderTarget(o, true, renderer.get()));
  if (!target->InitDefault(o, this)) {
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
  renderer_.reset(renderer.release());
  return true;
}

bool D3D11SwapChain::resize(int width, int height) {
  CHECK(false);
  return true;
}

void D3D11SwapChain::Present() {
  DCHECK(NULL != swap_chain_);
  swap_chain_->Present(0, 0);
}

bool D3D11SwapChain::reset(int width, int height) {
  CHECK(false);
  return true;
}

}  // namespace azer
