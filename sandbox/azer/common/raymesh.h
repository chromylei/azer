#pragma once

#include "azer/render/render.h"
#include "azer/math/math.h"
#include "sandbox/azer/common/camera.h"

class RayMesh {
 public:
  void Init();
  void Render(Camera* camera, const azer::Vector3& p1, const azer::Vector3& p2);
 private:
  static const azer::VertexDesc::Desc kVertexDescs[];
  static const int kDescNum;
  
  azer::GpuConstantsTablePtr vs_table_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
};
