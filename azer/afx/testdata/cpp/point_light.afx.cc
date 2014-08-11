 /**
 * Generated by afx
 * dont edit by hande
 */
#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"

#include "azer/render/render.h"

const azer::VertexDesc::Desc PoingLightEffect::kVertexDesc[] = {
  {"POSITION", 0, azer::kVec4},
  {"COORDTEX", 0, azer::kVec2},
  {"NORMAL", 0, azer::kVec4},
};

const int PoingLightEffect::kVertexDescNum = arraysize(PoingLightEffect::kVertexDesc);

PoingLightEffect::PoingLightEffect(const std::vector<std::string>& sources, azer::RenderSystem* rs) 
  : azer::Effect(rs) 
  , sources_(sources) {
  DCHECK(sources.size() == azer::kRenderPipelineStageNum);
  DCHECK(!sources[azer::kVertexStage].empty());
  DCHECK(!sources[azer::kPixelStage].empty());
  Init();
}

PoingLightEffect::~PoingLightEffect() {
}

void PoingLightEffect::Init() {
  InitTechnique();
  // generate GpuTable init for stage azer::kVertexStage
  azer::GpuConstantsTable::Desc vs_table_desc[] = {
    azer::GpuConstantsTable::Desc("pvw", azer::GpuConstantsType::kMatrix4,
         offsetof(vs_cbuffer, pvw), 1),
    azer::GpuConstantsTable::Desc("world", azer::GpuConstantsType::kMatrix4,
         offsetof(vs_cbuffer, world), 1),
  };
  gpu_table_[azer::kVertexStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc));
  // generate GpuTable init for stage azer::kPixelStage
  azer::GpuConstantsTable::Desc ps_table_desc[] = {
    azer::GpuConstantsTable::Desc("light", azer::GpuConstantsType::kSelfDefined,
         offsetof(ps_cbuffer, sizeof(PointLight), light), 1),
    azer::GpuConstantsTable::Desc("diffuse", azer::GpuConstantsType::kVector4,
         offsetof(ps_cbuffer, diffuse), 1),
  };
  gpu_table_[azer::kPixelStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc));
}

void PoingLightEffect::InitTechnique() {
  technique_.reset(render_system_->CreateTechnique());
  vertex_desc_ptr_.reset(new azer::VertexDesc(kVertexDesc, kVertexDescNum));
  const std::string& vs_shader_source = sources_[azer::kVertexStage];
  DCHECK(!vs_shader_source.empty());
  azer::GpuProgramPtr vs_gpup_ptr(render_system_->CreateVertexGpuProgram(
      vertex_desc_ptr_, vs_shader_source));
  DCHECK(vs_gpup_ptr.get() != NULL);
  technique_->AddGpuProgram(vs_gpup_ptr);
  {
    const std::string& source = sources_[azer::kPixelStage];
    DCHECK(!source.empty());
    azer::GpuProgramPtr gpup_ptr(render_system_->CreateGpuProgram(
        azer::kPixelStage, source));
    DCHECK(gpup_ptr.get() != NULL);
    technique_->AddGpuProgram(gpup_ptr);
  }
  technique_->Use(render_system_->GetDefaultRenderer());
}

void PoingLightEffect::UseTexture(azer::Renderer* renderer) {
}
void PoingLightEffect::SetMaterial(azer::MaterialPtr material) {
}


