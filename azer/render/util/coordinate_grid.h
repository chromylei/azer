#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/effect.h"
#include "azer/render/gpu_constants_table.h"
#include "azer/render/indices_buffer.h"
#include "azer/render/technique.h"
#include "azer/render/vertex_buffer.h"

namespace azer {
class AZER_EXPORT CoordinateGridEffect : public azer::Effect {
 public:
  CoordinateGridEffect(azer::RenderSystem* rs); 
#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 vp;
  };
  struct ps_cbuffer {
    azer::Vector4 diffuse;
  };
#pragma pack(pop)

  struct Vertex {
    azer::Vector4 position;
    Vertex(const azer::Vector4& in_position)
        : position(in_position)
    {}
    Vertex() {}
  };

  void SetWVP(const azer::Matrix4& value) {
    azer::GpuConstantsTable* tb = gpu_table_[(int)azer::kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, (void*)&value,  sizeof(azer::Matrix4));
  }

  void SetGridDiffuse(const azer::Vector4& value) {
    azer::GpuConstantsTable* tb = gpu_table_[(int)azer::kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, (void*)&value,  sizeof(azer::Vector4));
  }

  azer::VertexDescPtr GetVertexDesc() { return technique_->GetVertexDesc();}
  static const int kVertexDescNum;
  static const azer::VertexDesc::Desc kVertexDesc[];
 protected:
  void Init(RenderSystem* rs);
  virtual void UseTexture(azer::Renderer* renderer) OVERRIDE;
  DISALLOW_COPY_AND_ASSIGN(CoordinateGridEffect);
};

class AZER_EXPORT CoordinateGrid {
 public:
  CoordinateGrid(RenderSystem* rs, int width, int height, int num)
      : render_system_(rs)
      , kWidth(width), kHeight(height), kNum(num) {
    Init();
  }

  ~CoordinateGrid();

  void Render(Renderer* rs);
  void SetGridDiffuse(const Vector4& grid_diffuse);
  void SetProjViewMat(const Matrix4& mat);
 private:
  void Init();

  std::unique_ptr<CoordinateGridEffect> effect_ptr_;
  VertexDataPtr data_;
  VertexBufferPtr vb_;
  RenderSystem* render_system_;
  const int kWidth, kHeight, kNum;
  DISALLOW_COPY_AND_ASSIGN(CoordinateGrid);
};
}  // namespace azer
