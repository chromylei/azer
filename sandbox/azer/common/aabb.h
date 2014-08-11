#pragma once

#include "azer/render/render.h"
#include "sandbox/azer/common/camera.h"

class AABBMesh {
 public:
  AABBMesh(azer::RenderSystem* rs);
  ~AABBMesh() {}

  /**
   * 在渲染时，由于 AxisAlignedBoundingBox 有可能已经发生了变化
   * 因此为了使 AABBMesh 能够套住 Mesh，需要对 box 进行变换才行
   */
  void Render(azer::RenderSystem* rs, const azer::AxisAlignedBoundingBox& box,
              const Camera& camera);
 private:
  void Init(azer::RenderSystem* rs);
  void InitVertexBuffer(azer::RenderSystem* rs);
  void InitIndicesBuffer(azer::RenderSystem* rs);
  void InitShader(azer::RenderSystem* rs);
  
  azer::TechniquePtr tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::GpuConstantsTablePtr vs_table_;

  azer::BlendingPtr blending_;

  static const azer::VertexDesc::Desc kVertexDescs[];
  static const int kDescNum;
  DISALLOW_COPY_AND_ASSIGN(AABBMesh);
};
