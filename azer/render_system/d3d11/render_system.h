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
  D3D11RenderSystem();
  ~D3D11RenderSystem();

  virtual bool Init(WindowHost* window);
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

  virtual void Present() OVERRIDE;

  virtual AzerEGLInterface* GetEGLInterface() OVERRIDE;

  /**
   * direct3d relevent
   */
  ID3D11Device* GetDevice();
  ID3D11DeviceContext* GetContext();
  void ResetRenderTarget();
  D3D_FEATURE_LEVEL feature_level() const;
 protected:
  void GetDriverCapability();

  static const StringType& name_;
  static const StringType& short_name_;
  DISALLOW_COPY_AND_ASSIGN(D3D11RenderSystem);
};

inline ID3D11DeviceContext* D3D11RenderSystem::GetContext() {
  DCHECK(NULL != swap_chain_.get());
  D3D11SwapChain* swap_chain = (D3D11SwapChain*)swap_chain_.get();
  return swap_chain->GetContext();
}
inline ID3D11Device* D3D11RenderSystem::GetDevice() {
  DCHECK(NULL != swap_chain_.get());
  D3D11SwapChain* swap_chain = (D3D11SwapChain*)swap_chain_.get();
  return swap_chain->GetDevice();
}

inline D3D_FEATURE_LEVEL D3D11RenderSystem::feature_level() const {
  DCHECK(NULL != swap_chain_.get());
  D3D11SwapChain* swap_chain = (D3D11SwapChain*)swap_chain_.get();
  return swap_chain->feature_level();
}
}  // namespace azer
