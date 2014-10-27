#pragma once

#include "azer/render_system/d3d11/angle/precompile.h"

#include "base/logging.h"
#include "libGLESv2/renderer/d3d11/SwapChain11.h"
#include "azer/render_system/d3d11/swap_chain.h"

namespace azer {

class D3D11RenderSystem;
/**
 * 由于 azer::RenderSystem 已经完成了 swapchain 的创建
 * 因此此处完成的功能全部使用 azer:: 来完成
 */
class AzerSwapChain : public rx::SwapChain11 {
 public:
  AzerSwapChain(rx::Renderer11 *renderer, HWND window, HANDLE shareHandle,
                GLenum backBufferFormat, GLenum depthBufferFormat);
  ~AzerSwapChain();

  virtual EGLint reset(EGLint backbufferWidth, EGLint backbufferHeight, EGLint swapInterval);
  virtual EGLint swapRect(EGLint x, EGLint y, EGLint width, EGLint height);
  virtual void recreate();

  virtual ID3D11Texture2D *getOffscreenTexture();
  virtual ID3D11RenderTargetView *getRenderTarget();
  virtual ID3D11ShaderResourceView *getRenderTargetShaderResource();

  virtual ID3D11Texture2D *getDepthStencilTexture();
  virtual ID3D11DepthStencilView *getDepthStencil();
 private:
  SwapChainPtr swap_chain_;
  DISALLOW_COPY_AND_ASSIGN(AzerSwapChain);
};
}  // namespace azer
