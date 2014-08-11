#include "sandbox/azer/common/normal_render.h"
#include "base/basictypes.h"

using azer::VertexDesc;
const VertexDesc::Desc kNormalVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec3},
};

void NormalRender::InitNormalShader(azer::VertexDescPtr desc) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc, vs_prog_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, ps_prog_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->Use();
  azer::GpuConstantsTable::Desc ct_desc[] = {
      azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(ct_desc), ct_desc));
}


void NormalRender::Init(const Vertex* vertices, int vnum) {
  vnum_ = vnum;
  ::base::FilePath normal_vs_path(TEXT("sandbox/azer/common/nvs.hlsl"));
  ::base::FilePath normal_ps_path(TEXT("sandbox/azer/common/nps.hlsl"));
  CHECK(::base::ReadFileToString(normal_vs_path, &vs_prog_));
  CHECK(::base::ReadFileToString(normal_ps_path, &ps_prog_));

  azer::RenderSystem* rs = azer::RenderSystem::Current();
  const int num = vnum * 2;
  normal_vertex_.reset(new azer::Vector3[num]);
  for (int i = 0; i < vnum; ++i) {
    normal_vertex_.get()[i * 2] = vertices[i].position;
    normal_vertex_.get()[i * 2 + 1] = vertices[i].position
        + vertices[i].normal * 1.0;
  }

  azer::VertexDescPtr desc(new azer::VertexDesc(kNormalVertexDescs,
                                                ARRAYSIZE(kNormalVertexDescs)));
  data_.reset(new azer::VertexData(desc, num));
  memcpy(data_->pointer(), (uint8*)normal_vertex_.get(), num * sizeof(azer::Vector3));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  InitNormalShader(desc);
}


void NormalRender::Render(const azer::Matrix4& wvp) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->Render(vb_.get(), azer::kLineList, vnum_ * 2);
}
