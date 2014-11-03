#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

#include <string>

#include "base/logging.h"
#include "azer/render/render_system.h"
#include "azer/render/gpu_program.h"
#include "azer/render_system/d3d11/swap_chain.h"

namespace azer {

class WindowHost;
class VertexBuffer;
class D3D11RenderTarget;
class D3D11DepthBuffer;

class D3D11RenderSystem : public RenderSystem {
 public:
  D3D11RenderSystem(WindowHost* host);
  ~D3D11RenderSystem();

  bool Init();
  virtual const StringType& name() const OVERRIDE;
  virtual const StringType& short_name() const OVERRIDE;

  virtual Renderer* CreateRenderer(const Texture::Options& opt) OVERRIDE;
  virtual Renderer* CreateDeferredRenderer(const Texture::Options& opt) OVERRIDE;
  virtual Blending* CreateBlending(const Blending::Desc& desc) OVERRIDE;
  virtual VertexBuffer* CreateVertexBuffer(const VertexBuffer::Options& opt,
                                           VertexData*) OVERRIDE;
  virtual IndicesBuffer* CreateIndicesBuffer(const IndicesBuffer::Options& opt,
                                             IndicesData*) OVERRIDE;
  virtual GpuConstantsTable* CreateGpuConstantsTable(
      int32 num, const GpuConstantsTable::Desc* desc) OVERRIDE;

  // texutre functions
  virtual Texture* CreateTexture(const Texture::Options& opt) OVERRIDE;
  virtual Texture* CreateTexture(const Texture::Options& opt,
                                 const Image* image) OVERRIDE;
  // virtual RenderTarget* CreateRenderTarget(const Texture::Options& opt);
  // virtual DepthBuffer* CreateDepthBuffer(const Texture::Options& opt);

  virtual VertexGpuProgram* CreateVertexGpuProgram(VertexDescPtr desc,
                                                   const std::string& str) OVERRIDE;
  virtual GpuProgram* CreateGpuProgram(RenderPipelineStage stage,
                                       const std::string& program) OVERRIDE;
  virtual Overlay* CreateOverlay(const gfx::RectF& rect) OVERRIDE;
  virtual Technique* CreateTechnique() OVERRIDE;

  virtual bool Present() OVERRIDE;
  virtual bool reset() OVERRIDE;

  virtual AzerEGLInterface* GetEGLInterface() OVERRIDE;

  /**
   * direct3d relevent
   */
  ID3D11Device* GetDevice();
  ID3D11DeviceContext* GetContext();
  IDXGIFactory* GetDxgiFactory();
  void ResetRenderTarget();
  D3D_FEATURE_LEVEL feature_level() const;
 protected:
  void GetDriverCapability();
  bool InitD3DDevice();
  bool InitDefaultRenderer();

  ID3D11Device* d3d_device_;
  ID3D11DeviceContext* d3d_context_;
  IDXGIFactory* dxgi_factory_;
  IDXGIAdapter* dxgi_adapter_;
  D3D_FEATURE_LEVEL feature_level_;
  static const StringType& name_;
  static const StringType& short_name_;
  DISALLOW_COPY_AND_ASSIGN(D3D11RenderSystem);
};

inline ID3D11DeviceContext* D3D11RenderSystem::GetContext() {
  DCHECK(NULL != d3d_context_);
  return d3d_context_;
}
inline ID3D11Device* D3D11RenderSystem::GetDevice() {
  DCHECK(NULL != d3d_device_);
  return d3d_device_;
}

inline IDXGIFactory* D3D11RenderSystem::GetDxgiFactory() {
  DCHECK(NULL != dxgi_factory_);
  return dxgi_factory_;
}

inline D3D_FEATURE_LEVEL D3D11RenderSystem::feature_level() const {
  DCHECK(NULL != feature_level_);
  return feature_level_;
}
}  // namespace azer
