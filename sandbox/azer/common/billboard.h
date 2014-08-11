#pragma once

#include <d3d11.h>
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "sandbox/azer/common/camera2.h"

class Billboard {
 public:
  void Init(const std::vector<azer::Vector3>& pos);

  void Render(azer::Texture* tex, const Camera2& camera, const azer::Matrix4& world);
 private:
  azer::VertexBufferPtr vb_;
  azer::TechniquePtr tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::GpuProgramPtr gs_;

  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
};
