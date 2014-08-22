#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

#include <string>

#include "base/logging.h"
#include "azer/render/render_system.h"
#include "azer/render/gpu_program.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/gpu_program.h"

namespace azer {

class WindowHost;
class VertexBuffer;
class D3D11RenderTarget;
class D3D11DepthBuffer;

class D3D11Renderer : public Renderer {
 public:
  D3D11Renderer(ID3D11DeviceContext* context, D3D11RenderSystem* rs);
  ~D3D11Renderer();

  virtual bool Init(const Texture::Options& opt) OVERRIDE;
  virtual void Reset() OVERRIDE;
  virtual void Use() OVERRIDE;
  virtual const std::string& name() const OVERRIDE;

  virtual bool IsDepthTestEnable() OVERRIDE;
  virtual void EnableDepthTest(bool enable) OVERRIDE;

  virtual FillMode GetFillMode(void) OVERRIDE;
  virtual void SetFillMode(FillMode mode) OVERRIDE;
  virtual CullingMode GetCullingMode(void) OVERRIDE;
  virtual void SetCullingMode(CullingMode mode) OVERRIDE;
  virtual FrontFace GetFrontFace() OVERRIDE;
  virtual void SetFrontFace(FrontFace mode) OVERRIDE;
  virtual void EnableMultisampleAntiAliasing(bool enable) OVERRIDE;
  virtual bool IsMultisampleAntiAliasingEnabled() OVERRIDE;
  virtual void EnableLineAntialiasing(bool enable) OVERRIDE;
  virtual bool IsLineAntialiasingEnabled() OVERRIDE;

  virtual void UseBlending(Blending* blending, float* factor, uint32 mask) OVERRIDE;
  virtual void ResetBlending() OVERRIDE;

  virtual void UseConstantsTable(RenderPipelineStage stage,
                                 GpuConstantsTable* table) OVERRIDE;

  // texutre functions
  virtual void UseTexture(RenderPipelineStage stage, int index, Texture* texture) OVERRIDE;
  virtual void ResetShader(RenderPipelineStage stage) OVERRIDE;

  virtual void Clear(const azer::Vector4& color) OVERRIDE;
  virtual void ClearDepthAndStencil(DepthBuffer::ClearFlag,
                                    float depth_val, int stencil_val) OVERRIDE;
  
  virtual void Draw(VertexBuffer* vb, PrimitiveTopology primitive,
                      int32 num_of_vertices, int32 first_vertex) OVERRIDE;
  virtual void DrawIndex(VertexBuffer* vb, IndicesBuffer* ib,
                         PrimitiveTopology primitive, int32 num_of_indices,
                         int32 first_indices, int32 index_base) OVERRIDE;
  virtual void DrawInstanced(int32 instance_num, VertexBuffer* vb,
                             PrimitiveTopology primitive,
                             int32 num_of_vertices, int32 first_vertex,
                             int32 instance_start_index) OVERRIDE;
  virtual void DrawIndexInstanced(int32 instance_num, VertexBuffer* vb,
                                  IndicesBuffer* ib, PrimitiveTopology primitive,
                                  int32 num_of_indices, int32 first_indices,
                                  int32 index_base,
                                  int32 instance_start_index) OVERRIDE;

  virtual void SetViewport(const Viewport& viewport) OVERRIDE;
  
  ID3D11DeviceContext* GetContext() {
    CHECK(NULL != d3d_context_);
    return d3d_context_;
  }

  ID3D11Device* GetDevice() {
    DCHECK(NULL != d3d11_render_system_);
    return d3d11_render_system_->GetDevice();
  }

  void ResetRenderTarget();
 protected:
  void InitRenderState();
  void InitForRenderTarget();
  void ResetTexture(RenderPipelineStage stage, int index);
  void SetShaderResource(RenderPipelineStage stage, uint32 first, uint32 num,
                         ID3D11ShaderResourceView** view);

  D3D11RenderSystem* d3d11_render_system_;
  Viewport viewport_;
  ID3D11DeviceContext* d3d_context_;
  static const std::string& name_;
  friend class RenderSystem;
  DISALLOW_COPY_AND_ASSIGN(D3D11Renderer);
};
}  // namespace azer
