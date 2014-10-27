#pragma once

#include <vector>
#include <string>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/indices_buffer.h"
#include "azer/render/blending.h"
#include "azer/render/depth_buffer.h"
#include "azer/render/gpu_program.h"
#include "azer/render/gpu_constants_table.h"
#include "azer/render/render_target.h"
#include "azer/render/render_system_enum.h"
#include "azer/render/texture.h"
#include "azer/render/vertex_buffer.h"

namespace azer {

class RenderSystem;

class AZER_EXPORT Renderer {
 public:
  Renderer(int num_of_rt, RenderSystem* rs)
      : render_system_(rs)
      , kNumOfRenderTarget(num_of_rt) {
    targets_.resize(num_of_rt);
  }
  virtual ~Renderer() {}

  virtual const std::string& name() const = 0;

  virtual void Use() = 0;

  virtual bool IsDepthTestEnable() = 0;
  virtual void EnableDepthTest(bool enable) = 0;

  virtual FillMode GetFillMode(void) = 0;
  virtual void SetFillMode(FillMode mode) = 0;
  virtual CullingMode GetCullingMode(void) = 0;
  virtual void SetCullingMode(CullingMode mode) = 0;
  virtual FrontFace GetFrontFace() = 0;
  virtual void SetFrontFace(FrontFace mode) = 0;
  virtual void EnableMultisampleAntiAliasing(bool enable) = 0;
  virtual bool IsMultisampleAntiAliasingEnabled() = 0;
  virtual void EnableLineAntialiasing(bool enable) = 0;
  virtual bool IsLineAntialiasingEnabled() = 0;

  virtual void UseBlending(Blending* blending, float* factor,
                           uint32 mask = 0xffffffff) = 0;
  
  virtual void ResetBlending() = 0;

  virtual void UseTexture(RenderPipelineStage stage, int index, Texture* texture) = 0;

  virtual void UseConstantsTable(RenderPipelineStage stage,
                                 GpuConstantsTable* table) = 0;
  virtual void Reset() = 0;

  virtual void Clear(const azer::Vector4& color) = 0;

  virtual void ClearDepthAndStencil(
      DepthBuffer::ClearFlag flag = DepthBuffer::kClearAll,
      float depth_val = 1.0,int stencil_val = 0) = 0;

  virtual void ResetShader(RenderPipelineStage stage) = 0;

  virtual void Draw(VertexBuffer* vb, PrimitiveTopology primitive,
                    int32 num_of_vertices = -1, int32 first_vertex = 0) = 0;
  virtual void DrawIndex(VertexBuffer* vb, IndicesBuffer* ib,
                         PrimitiveTopology primitive, int32 num_of_indices = -1,
                         int32 first_indices = 0, int32 index_base = 0) = 0;
  virtual void DrawInstanced(int32 instance_num, VertexBuffer* vb,
                             PrimitiveTopology primitive, int32 num_of_vertices = -1,
                             int32 first_vertex = 0,
                             int32 instance_start_index = 0) = 0;
  virtual void DrawIndexInstanced(int32 instance_num, VertexBuffer* vb,
                                  IndicesBuffer* ib, PrimitiveTopology primitive,
                                  int32 num_of_indices = -1,
                                  int32 first_indices = 0, int32 index_base = 0,
                                  int32 instance_start_index = 0) = 0;
  
  struct Viewport {
    int32 left;
    int32 top;
    int32 width;
    int32 height;
    int32 min_depth;
    int32 max_depth;

    Viewport(int32 l, int32 t, int32 w, int32 h, float mindepth = 0.0f,
             float maxdepth = 1.0f)
        : left(l), top(t), width(w), height(h)
        , min_depth(mindepth), max_depth(maxdepth) {
    }

    Viewport()
        : left(0), top(0), width(800), height(600)
        , min_depth(0.0f), max_depth(1.0f) {
    }
  };
  virtual void SetViewport(const Viewport& viewport) = 0;

  DepthBuffer* GetDepthBuffer() { return depth_.get();}
  RenderTarget* GetRenderTarget() { return targets_[0].get();}
  typedef std::vector<std::unique_ptr<RenderTarget> > RenderTargetVec;
  RenderTargetVec* GetAllRenderTargets() { return &targets_;}
  RenderSystem* GetRenderSystem() { return render_system_;}
 protected:
  RenderSystem* render_system_;
  std::unique_ptr<DepthBuffer> depth_;
  RenderTargetVec targets_;
  const int kNumOfRenderTarget;
  DISALLOW_COPY_AND_ASSIGN(Renderer);
};

typedef std::shared_ptr<Renderer> RendererPtr;
}  // namespace azer
