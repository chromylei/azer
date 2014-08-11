#include "sandbox/azer/common/aabb.h"
#include "base/file_util.h"

using azer::RenderSystem;

// class AABBMesh
const azer::VertexDesc::Desc AABBMesh::kVertexDescs[] = {
  {"POSITION", azer::VertexDesc::kPosition, azer::kVec3},
};
const int AABBMesh::kDescNum = arraysize(AABBMesh::kVertexDescs);

AABBMesh::AABBMesh(RenderSystem* rs) {
  Init(rs);
}

void AABBMesh::Init(RenderSystem* rs) {
  InitVertexBuffer(rs);
  InitIndicesBuffer(rs);
  InitShader(rs);

  azer::Blending::Desc blending_desc;
  blending_desc.src = azer::Blending::kSrcAlpha;
  blending_desc.dest = azer::Blending::kSrcInvColor;
  blending_desc.oper = azer::Blending::kAdd;
  blending_.reset(rs->CreateBlending(blending_desc));
}

void AABBMesh::InitVertexBuffer(RenderSystem* rs) {
  using azer::VertexDesc;
  using azer::VertexDescPtr;
  const int kVertexNum = 8;
  azer::VertexDescPtr desc(new VertexDesc(kVertexDescs, kDescNum));
  azer::VertexDataPtr data(new azer::VertexData(desc, kVertexNum));
  azer::Vector3* begin_vptr = (azer::Vector3*)data->pointer();
  azer::Vector3* vptr = begin_vptr;
  //
  *vptr++ = azer::Vector3(-0.5f,  0.5f,  0.5f);
  *vptr++ = azer::Vector3(-0.5f,  0.5f, -0.5f);
  *vptr++ = azer::Vector3( 0.5f,  0.5f, -0.5f);
  *vptr++ = azer::Vector3( 0.5f,  0.5f,  0.5f);

  *vptr++ = azer::Vector3(-0.5f, -0.5f,  0.5f);
  *vptr++ = azer::Vector3(-0.5f, -0.5f, -0.5f);
  *vptr++ = azer::Vector3( 0.5f, -0.5f, -0.5f);
  *vptr++ = azer::Vector3( 0.5f, -0.5f,  0.5f);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));
}

void AABBMesh::InitIndicesBuffer(RenderSystem* rs) {
  using azer::IndicesDataPtr;
  using azer::IndicesData;
  const int kTriangleNum = 12;
  IndicesDataPtr idata(new IndicesData(kTriangleNum * 3, IndicesData::kUint32,
                                       IndicesData::kMainMemory));
  int32 indices[kTriangleNum * 3] = {
    0,3,1, 1,3,2,  // top
    4,5,7, 7,5,6,  // bottom
    1,2,5, 5,2,6,  // front
    0,4,3, 3,4,7,  // back
    0,1,4, 4,1,5,  // left
    2,3,7, 2,7,6,  // right
  };
  int32* cur = (int32*)idata->pointer();
  memcpy(cur, indices, sizeof(int32) * kTriangleNum * 3);
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));
}


void AABBMesh::InitShader(RenderSystem* rs) {
  std::string vertex_shader;
  std::string fragment_shader;
  ::base::FilePath vertex_shader_path(TEXT("sandbox/azer/common/aabb.vs.hlsl"));
  ::base::FilePath pixel_shader_path(TEXT("sandbox/azer/common/aabb.ps.hlsl"));
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader));
  
  
  vs_.reset(rs->CreateVertexGpuProgram(vb_->desc(), vertex_shader));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader));
  tech_.reset(rs->CreateEffect());
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->Use();

  using azer::GpuConstantsType;
  using azer::GpuConstantsTable;
  GpuConstantsTable::Desc vs_desc[] = {
    GpuConstantsTable::Desc("WVP", GpuConstantsType::kMatrix4, 1),
    // GpuConstantsTable::Desc("WORLD", GpuConstantsType::kMatrix4, 1),
  };
  
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(vs_desc), vs_desc));
}

void AABBMesh::Render(RenderSystem* rs, const azer::AxisAlignedBoundingBox& box,
                      const Camera& camera) {
  // 根据 bbox 计算缩放的坐标进行缩放及移动
  // 以保证 AABB 的 Mesh 能够包裹住 box
  azer::Vector3 pos = (box.min() + box.max()) / 2.0f;
  azer::Vector3 scale = (box.max() - box.min()) * 1.05f;
  XMMATRIX Scale = XMMatrixScaling(scale.x, scale.y, scale.z);
  XMMATRIX Trans = XMMatrixTranslation(pos.x, pos.y, pos.z);
  XMMATRIX World = Scale * Trans;
  XMMATRIX WVP = World * camera.view() * camera.projection();
  XMMATRIX twvp = XMMatrixTranspose(WVP);

  rs->UseBlending(blending_.get(), 0);
  vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  tech_->Use();
  rs->SetCullingMode(azer::kCullBack);
  rs->SetFillMode(azer::kWireFrame);
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, 36);
  rs->SetFillMode(azer::kSolid);
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, 36);
  rs->ResetBlending();
}

