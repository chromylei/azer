#include "azer/render_system/d3d11/angle/swap_chain.h"

#include "azer/render_system/d3d11/angle/renderer.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/render_target.h"
#include "azer/render_system/d3d11/depth_buffer.h"
#include "azer/render_system/d3d11/texture.h"

#include "libGLESv2/renderer/d3d11/renderer11_utils.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"
#include "libGLESv2/renderer/d3d11/SwapChain11.h"
#include "libGLESv2/renderer/d3d11/shaders/compiled/passthrough11vs.h"
#include "libGLESv2/renderer/d3d11/shaders/compiled/passthroughrgba11ps.h"

namespace azer {
AzerSwapChain::AzerSwapChain(rx::Renderer11 *renderer, HWND window,
                             HANDLE shareHandle,
                             GLenum backBufferFormat,
                             GLenum depthBufferFormat)
    : rx::SwapChain11(renderer, window, shareHandle,
                      backBufferFormat, depthBufferFormat) {
  AzerRenderer* ar = (AzerRenderer*)renderer;
  D3D11RenderSystem* rs = ar->GetRenderSystem();
  swap_chain_ = rs->GetSwapChain();
  D3D11SwapChain* sc = ((D3D11SwapChain*)swap_chain_.get());
  mSwapChain = sc->GetD3D11SwapChain();
}

AzerSwapChain::~AzerSwapChain() {
}

EGLint AzerSwapChain::reset(EGLint backbufferWidth, EGLint backbufferHeight,
                            EGLint swapInterval) {
  return EGL_SUCCESS;
}

EGLint AzerSwapChain::swapRect(EGLint x, EGLint y, EGLint width, EGLint height) {
  if (!mSwapChain)
  {
    return EGL_SUCCESS;
  }

  ID3D11Device *device = mRenderer->getDevice();
  ID3D11DeviceContext *deviceContext = mRenderer->getDeviceContext();

  // Set vertices
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HRESULT result = deviceContext->Map(mQuadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    return EGL_BAD_ACCESS;
  }

  d3d11::PositionTexCoordVertex *vertices = static_cast<d3d11::PositionTexCoordVertex*>(mappedResource.pData);

  // Create a quad in homogeneous coordinates
  float x1 = (x / float(mWidth)) * 2.0f - 1.0f;
  float y1 = (y / float(mHeight)) * 2.0f - 1.0f;
  float x2 = ((x + width) / float(mWidth)) * 2.0f - 1.0f;
  float y2 = ((y + height) / float(mHeight)) * 2.0f - 1.0f;

  float u1 = x / float(mWidth);
  float v1 = y / float(mHeight);
  float u2 = (x + width) / float(mWidth);
  float v2 = (y + height) / float(mHeight);

  d3d11::SetPositionTexCoordVertex(&vertices[0], x1, y1, u1, v1);
  d3d11::SetPositionTexCoordVertex(&vertices[1], x1, y2, u1, v2);
  d3d11::SetPositionTexCoordVertex(&vertices[2], x2, y1, u2, v1);
  d3d11::SetPositionTexCoordVertex(&vertices[3], x2, y2, u2, v2);

  deviceContext->Unmap(mQuadVB, 0);

  static UINT stride = sizeof(d3d11::PositionTexCoordVertex);
  static UINT startIdx = 0;
  deviceContext->IASetVertexBuffers(0, 1, &mQuadVB, &stride, &startIdx);

  // Apply state
  deviceContext->OMSetDepthStencilState(NULL, 0xFFFFFFFF);

  static const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  deviceContext->OMSetBlendState(NULL, blendFactor, 0xFFFFFFF);

  deviceContext->RSSetState(NULL);

  // Apply shaders
  deviceContext->IASetInputLayout(mPassThroughIL);
  deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  deviceContext->VSSetShader(mPassThroughVS, NULL, 0);
  deviceContext->PSSetShader(mPassThroughPS, NULL, 0);
  deviceContext->GSSetShader(NULL, NULL, 0);

  // Apply render targets
  mRenderer->setOneTimeRenderTarget(mBackBufferRTView);

  // Set the viewport
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = mWidth;
  viewport.Height = mHeight;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  deviceContext->RSSetViewports(1, &viewport);

  // Apply textures
  deviceContext->PSSetShaderResources(0, 1, &mOffscreenSRView);
  deviceContext->PSSetSamplers(0, 1, &mPassThroughSampler);

  // Draw
  deviceContext->Draw(4, 0);

#if ANGLE_FORCE_VSYNC_OFF
  result = mSwapChain->Present(0, 0);
#else
  result = mSwapChain->Present(mSwapInterval, 0);
#endif

  if (result == DXGI_ERROR_DEVICE_REMOVED)
  {
    HRESULT removedReason = device->GetDeviceRemovedReason();
    ERR("Present failed: the D3D11 device was removed: 0x%08X", removedReason);
    return EGL_CONTEXT_LOST;
  }
  else if (result == DXGI_ERROR_DEVICE_RESET)
  {
    ERR("Present failed: the D3D11 device was reset from a bad command.");
    return EGL_CONTEXT_LOST;
  }
  else if (FAILED(result))
  {
    ERR("Present failed with error code 0x%08X", result);
  }

  // Unbind
  static ID3D11ShaderResourceView *const nullSRV = NULL;
  deviceContext->PSSetShaderResources(0, 1, &nullSRV);

  mRenderer->unapplyRenderTargets();
  mRenderer->markAllStateDirty();

  return EGL_SUCCESS;

}

void AzerSwapChain::recreate() {
}

ID3D11Texture2D *AzerSwapChain::getOffscreenTexture() {
  TexturePtr rtt = swap_chain_->GetRenderTarget()->GetTexture();
  return (ID3D11Texture2D*)(((D3D11Texture*)rtt.get())->GetResource());
}

ID3D11RenderTargetView *AzerSwapChain::getRenderTarget() {
  RenderTargetPtr rt = swap_chain_->GetRenderTarget();
  return ((D3D11RenderTarget*)rt.get())->GetD3D11RenderTargetView();
}

ID3D11ShaderResourceView *AzerSwapChain::getRenderTargetShaderResource() {
  TexturePtr rtt = swap_chain_->GetRenderTarget()->GetTexture();
  return ((D3D11Texture*)rtt.get())->GetResourceView();
}

ID3D11Texture2D *AzerSwapChain::getDepthStencilTexture() {
  DepthBufferPtr dbp = swap_chain_->GetDepthBuffer();
  TexturePtr rtt = swap_chain_->GetRenderTarget()->GetTexture();
  return (ID3D11Texture2D*)(((D3D11Texture*)rtt.get())->GetResource());
}

ID3D11DepthStencilView *AzerSwapChain::getDepthStencil() {
  DepthBufferPtr dbp = swap_chain_->GetDepthBuffer();
  return ((D3D11DepthBuffer*)dbp.get())->GetD3D11DepthStencilView();
}
}  // namespace azer
