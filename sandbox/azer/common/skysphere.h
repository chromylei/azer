#pragma once

#include "azer/math/math.h"
#include "azer/render/render.h"
#include "sandbox/azer/common/camera2.h"

class SkySphere {
 public:
  SkySphere(int LatLines = 10, int LongLines = 12)
      : kLatLines(LatLines)
      , kLongLines(LongLines)
      , kVertexNum((kLatLines - 2) * kLongLines + 2)
      , kIndicesNum(kLongLines * 3 * 2 * (kLatLines - 2)) {
  }

  struct Vertex {
    azer::Vector3 position;
  };

  void Init();
  void Render(const Camera2& camera);
 private:
  void InitVertex();
  void InitIndices();
  std::unique_ptr<Vertex[]> vertices_;
  std::unique_ptr<uint32[]> indices_;

  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::TexturePtr tex_;

  std::string vertex_shader_;
  std::string fragment_shader_;
  const int kLatLines;
  const int kLongLines;
  const int kVertexNum;
  const int kIndicesNum;
};
