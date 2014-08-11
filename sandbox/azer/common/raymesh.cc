#include "sandbox/azer/common/raymesh.h"

#include "base/file_util.h"

using azer::VertexDesc;
const VertexDesc::Desc RayMesh::kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec4},
};

const int RayMesh::kDescNum = arraysize(kVertexDescs);

namespace {
::base::FilePath vertex_shader_path(TEXT("sandbox/azer/common/raymesh.vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/common/raymesh.ps.hlsl"));
}

void RayMesh::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();

  using azer::GpuConstantsTable;
  GpuConstantsTable::Desc vt_desc[] = {
    GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(vt_desc), vt_desc));

  azer::VertexDescPtr vdesc(new azer::VertexDesc(kVertexDescs,
                                                 ARRAYSIZE(kVertexDescs)));

  std::string vertex_shader, fragment_shader;
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader));
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(vdesc, vertex_shader));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
}

void RayMesh::Render(Camera* camera, const azer::Vector3& p1,
                     const azer::Vector3& p2) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));

  azer::VertexDataPtr data(new azer::VertexData(desc, 2));
  azer::Vector4 *vertex = (azer::Vector4*)data->pointer();
  *vertex++ = azer::Vector4(p1, 1.0f);
  *vertex++ = azer::Vector4(p2, 1.0f);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));

  XMMATRIX WVP = camera->view() * camera->projection();
  XMMATRIX twvp = XMMatrixTranspose(WVP);
  vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  tech_->Use();
  rs->Render(vb_.get(), azer::kLineList, 2);
}
                     
