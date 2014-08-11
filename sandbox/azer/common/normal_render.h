#pragma once

#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"

class NormalRender {
 public:
  struct Vertex {
    azer::Vector4 position;
    azer::Vector3 normal;
  };
  void Init(const Vertex* vertices, int num);
  void Render(const azer::Matrix4& wvp);
 private:
  void NormalRender::InitNormalShader(azer::VertexDescPtr desc);
  std::unique_ptr<azer::Vector3[]> normal_vertex_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::VertexDataPtr  data_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexBufferPtr vb_;
  std::string vs_prog_;
  std::string ps_prog_;
  std::unique_ptr<azer::Technique> tech_;
  int vnum_;
};
