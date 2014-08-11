#include "sandbox/azer/common/billboard.h"
#include "sandbox/azer/common/camera.h"
#include "azer/render/render.h"
#include "base/file_util.h"

namespace {
::base::FilePath vertex_shader_path(TEXT("sandbox/azer/common/billboard.vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/common/billboard.ps.hlsl"));
::base::FilePath geo_shader_path(TEXT("sandbox/azer/common/billboard.gs.hlsl"));
}

const azer::VertexDesc::Desc kVertexDescs[] = {
  {"POSITION", azer::VertexDesc::kPosition, azer::kVec3},
};

void Billboard::Init(const std::vector<azer::Vector3>& pos) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();

  azer::VertexDescPtr vdesc(new azer::VertexDesc(kVertexDescs,
                                                 ARRAYSIZE(kVertexDescs)));
  azer::VertexDataPtr vdata(new azer::VertexData(vdesc, pos.size()));
  azer::Vector3* ptr = (azer::Vector3*)vdata->pointer();
  memcpy(ptr, &pos[0], sizeof(azer::Vector3) * pos.size());
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), vdata));

  std::string vertex_shader_prog, pixel_shader_prog, geo_shader_prog;
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_prog));
  CHECK(::base::ReadFileToString(pixel_shader_path, &pixel_shader_prog));
  CHECK(::base::ReadFileToString(geo_shader_path, &geo_shader_prog));
  vs_.reset(rs->CreateVertexGpuProgram(vdesc, vertex_shader_prog));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, pixel_shader_prog));
  gs_.reset(rs->CreateGpuProgram(azer::kGeometryStage, geo_shader_prog));
  tech_.reset(rs->CreateEffect());
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->AddGpuProgram(gs_);
  tech_->Use();

  azer::GpuConstantsTable::Desc dvs_desc[] = {
    azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
    azer::GpuConstantsTable::Desc("CPOS", azer::GpuConstantsType::kVector4, 1),
    azer::GpuConstantsTable::Desc("SIZE", azer::GpuConstantsType::kVector2, 1),
    // azer::GpuConstantsTable::Desc("CAMERA", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(dvs_desc), dvs_desc));
}

void Billboard::Render(azer::Texture* tex, const Camera2& camera,
                       const azer::Matrix4& world) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Matrix4 wvp = camera.projection() * camera.view() * world;
  azer::Vector4 camera_pos(camera.position(), 1.0f);
  azer::Vector2 size(1, 1);
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&camera_pos, sizeof(azer::Vector4));
  vs_table_->SetValue(2, (void*)&size, sizeof(azer::Vector2));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kGeometryStage,vs_table_.get());
  tech_->Use();
  rs->UseTexture(azer::kPixelStage, 0, tex);
  rs->Render(vb_.get(), azer::kPointList, 1);
}
