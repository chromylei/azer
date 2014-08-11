#pragma once

#include <d3d11.h>

#include "azer/render/render.h"
#include "sandbox/azer/common/camera2.h"
#include "sandbox/azer/common/heightmap.h"
#include "sandbox/azer/common/normal_render.h"

class Grid {
 public:
  Grid() {}

  void Init(const base::FilePath& filepath);
  void Render(azer::RenderSystem* rs, const azer::Matrix4& world, Camera2* camera,
              const azer::Light& light);
  
  azer::VertexDescPtr desc() { return vb_->desc();}
 public:
  static const azer::VertexDesc::Desc kVertexDescs[];
  static const int kDescNum;
 private:
  void InitVertex();
  void InitTechnique();

  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::TexturePtr tex_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  NormalRender nrender_;
  HeightMapInfo height_map_;
};
