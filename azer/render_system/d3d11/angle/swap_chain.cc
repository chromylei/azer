#include "azer/render_system/d3d11/angle/swap_chain.h"

namespace azer {
AzerSwapChain::AzerSwapChain(rx::Renderer11 *renderer, HWND window,
                             HANDLE shareHandle,
                             GLenum backBufferFormat,
                             GLenum depthBufferFormat)
    : rx::SwapChain11(renderer, window, shareHandle,
                      backBufferFormat, depthBufferFormat) {
}

AzerSwapChain::~AzerSwapChain() {
}

EGLint AzerSwapChain::reset(EGLint backbufferWidth, EGLint backbufferHeight,
                            EGLint swapInterval) {

  return EGL_SUCCESS;
}

EGLint AzerSwapChain::swapRect(EGLint x, EGLint y, EGLint width, EGLint height) {
  return EGL_SUCCESS;
}

void AzerSwapChain::recreate() {
}

ID3D11Texture2D *AzerSwapChain::getOffscreenTexture() {
  return NULL;
}

ID3D11RenderTargetView *AzerSwapChain::getRenderTarget() {
  return NULL;
}

ID3D11ShaderResourceView *AzerSwapChain::getRenderTargetShaderResource() {
  return NULL;
}

ID3D11Texture2D *AzerSwapChain::getDepthStencilTexture() {
  return NULL;
}

ID3D11DepthStencilView *AzerSwapChain::getDepthStencil() {
  return NULL;
}
}  // namespace azer
