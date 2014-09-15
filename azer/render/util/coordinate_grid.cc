#include "azer/render/util/coordinate_grid.h"

#include "azer/math/math.h"

#include "base/files/file_path.h"
#include "base/file_util.h"
#include "azer/resources/resource_manager.h"

namespace azer {
// class CoordinateGridEffect
const VertexDesc::Desc CoordinateGridEffect::kVertexDesc[] = {
  {"POSITION", 0, kVec4},
};
const int CoordinateGridEffect::kVertexDescNum =
    arraysize(CoordinateGridEffect::kVertexDesc);

CoordinateGridEffect::CoordinateGridEffect(RenderSystem* rs)
    : Effect(rs) {
  Init(rs);
}

void CoordinateGridEffect::Init(RenderSystem* rs) {
  technique_ = ResourceManager::GetCommonResourceManager()->GetTechnique(
      AZER_LITERAL("technique/coordinate_grid_effect.tech"), rs);
 
 
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("vp", azer::GpuConstantsType::kMatrix4,
                            offsetof(CoordinateGridEffect::vs_cbuffer, vp), 1), 
  };
  gpu_table_[azer::kVertexStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc));
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("diffuse", azer::GpuConstantsType::kVector4,
                            offsetof(CoordinateGridEffect::ps_cbuffer, diffuse), 1), 
  };
  gpu_table_[azer::kPixelStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc));
}

void CoordinateGridEffect::UseTexture(azer::Renderer* renderer) {
}


// class CoordinateGrid
CoordinateGrid::~CoordinateGrid() {
}

void CoordinateGrid::Init() {
  std::vector<std::string> vec;
  vec.resize(kRenderPipelineStageNum);
  CoordinateGridEffect* effect = new CoordinateGridEffect(render_system_);
  effect_ptr_.reset(effect);
  data_.reset(new VertexData(effect->GetVertexDesc(), (kNum + 1) * 4));
  CoordinateGridEffect::Vertex* vertex =
      (CoordinateGridEffect::Vertex*)data_->pointer();
  float row_width = (float)kHeight / (float)kNum;
  for (int i = 0; i < kNum + 1; ++i) {
    float z = (float)kHeight / 2.0f - row_width * i;
    vertex->position = Vector4(-(float)kWidth / 2.0f, 0.0f, z, 1.0f);
    vertex++;
    vertex->position = Vector4((float)kWidth / 2.0f, 0.0f, z, 1.0f);
    vertex++;
  }

  float col_width = (float)kHeight / (float)kNum;
  for (int i = 0; i < kNum + 1; ++i) {
    float x  = (float)kWidth / 2.0f - col_width * i;
    vertex->position = Vector4(x, 0.0f, -(float)kHeight / 2.0f, 1.0f);
    vertex++;
    vertex->position = Vector4(x, 0.0f, (float)kHeight / 2.0f, 1.0f);
    vertex++;
  }
  vb_.reset(render_system_->CreateVertexBuffer(VertexBuffer::Options(), data_.get()));
}

void CoordinateGrid::Render(Renderer* renderer) {
  effect_ptr_->Use(renderer);
  renderer->Draw(vb_.get(), kLineList, (kNum + 1) * 4);
}

void CoordinateGrid::SetGridDiffuse(const Vector4& grid_diffuse) {
  effect_ptr_->SetGridDiffuse(grid_diffuse);
}

void CoordinateGrid::SetProjViewMat(const Matrix4& mat) {
  effect_ptr_->SetWVP(mat);
}
}  // namespace azer
