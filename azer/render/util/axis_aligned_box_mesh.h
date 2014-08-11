#pragma once

#include "azer/base/render_export.h"
#include "azer/render/effect.h"
#include "azer/render/gpu_constants_table.h"
#include "azer/render/indices_buffer.h"
#include "azer/render/technique.h"
#include "azer/render/vertex_buffer.h"
#include "azer/render/axis_aligned_box.h"
#include "base/basictypes.h"
#include "base/threading/non_thread_safe.h"

namespace azer {

class AZER_EXPORT AABBEffect : public Effect {
 public:
  AABBEffect(RenderSystem* rs);
#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 vp;
  };
  struct ps_cbuffer {
    azer::Vector4 diffuse;
  };
#pragma pack(pop)
  void SetWVP(const Matrix4& wvp) {
    azer::GpuConstantsTable* tb = gpu_table_[(int)azer::kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, (void*)&wvp,  sizeof(azer::Matrix4));
  }

  void SetDiffuse(const azer::Vector4& value) {
    azer::GpuConstantsTable* tb = gpu_table_[(int)azer::kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, (void*)&value,  sizeof(azer::Vector4));
  }

  VertexDescPtr GetVertexDesc() {
    DCHECK(NULL != technique_.get());
    return technique_->GetVertexDesc();
  }
 private:
  virtual void UseTexture(Renderer* renderer) OVERRIDE {}
  void Init(RenderSystem* rs);
  DISALLOW_COPY_AND_ASSIGN(AABBEffect);
};

/**
 * Render AABB
 * 注意:
 * 由于 bounding box 的渲染过程是用了 blending, 因此必须首先渲染真正的物体
 * 而后在进行 AABBMesh 的渲染
 * 最后进行 AABBMesh 的渲染也可以
 */
class AZER_EXPORT AABBMesh : public base::NonThreadSafe {
 public:
  ~AABBMesh();

  static AABBMesh* GetInstance(RenderSystem* rs);
  //
  // 在渲染时，由于 AxisAlignedBox 有可能已经发生了变化
  // 因此为了使 AABBMesh 能够套住 Mesh，需要对 box 进行变换才行
  //
  AABBEffect* effect() { return effect_.get();}
  void Render(Renderer* renderer);
 private:
  AABBMesh(RenderSystem* rs);

  void Init(RenderSystem* rs);
  void InitVertexBuffer(RenderSystem* rs);
  void InitIndicesBuffer(RenderSystem* rs);
  
  VertexBufferPtr vb_;
  IndicesBufferPtr ib_;
  IndicesBufferPtr edge_ib_;
  std::unique_ptr<AABBEffect> effect_;
  DISALLOW_COPY_AND_ASSIGN(AABBMesh);
};
}  // namespace azer
