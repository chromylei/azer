#include "azer/render/util/axis_aligned_box_mesh.h"

#include "azer/render/render.h"
#include "azer/resources/resource_manager.h"
#include "base/lazy_instance.h"

namespace azer {

AABBEffect::AABBEffect(RenderSystem* rs)
    : Effect(rs) {
  Init(rs);
}

void AABBEffect::Init(RenderSystem* rs) {
  technique_ = ResourceManager::GetCommonResourceManager()->GetTechnique(
      AZER_LITERAL("technique/bounding_volumn.tech"), rs);

  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("vp", azer::GpuConstantsType::kMatrix4,
                            offsetof(AABBEffect::vs_cbuffer, vp), 1), 
  };
  gpu_table_[azer::kVertexStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc));
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("diffuse", azer::GpuConstantsType::kVector4,
                            offsetof(AABBEffect::ps_cbuffer, diffuse), 1), 
  };
  gpu_table_[azer::kPixelStage].reset(render_system_->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc));
}

// class AABBMesh
AABBMesh* AABBMesh::GetInstance(RenderSystem* rs) {
  static AABBMesh* inst = NULL;
  if (inst == NULL) {
    inst = new AABBMesh(rs);
  }

  return inst;
}

AABBMesh::AABBMesh(RenderSystem* rs) {
  Init(rs);
}

void AABBMesh::Init(RenderSystem* rs) {
  effect_.reset(new AABBEffect(rs));
  InitVertexBuffer(rs);
  InitIndicesBuffer(rs);
}

void AABBMesh::InitVertexBuffer(RenderSystem* rs) {
  const int kVertexNum = 8;
  VertexDataPtr data(new VertexData(effect_->GetVertexDesc(), kVertexNum));
  Vector4* begin_vptr = (Vector4*)data->pointer();
  Vector4* vptr = begin_vptr;
  //
  *vptr++ = azer::Vector4(-0.5f,  0.5f,  0.5f, 1.0f);
  *vptr++ = azer::Vector4(-0.5f,  0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f,  0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f,  0.5f,  0.5f, 1.0f);

  *vptr++ = azer::Vector4(-0.5f, -0.5f,  0.5f, 1.0f);
  *vptr++ = azer::Vector4(-0.5f, -0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f, -0.5f, -0.5f, 1.0f);
  *vptr++ = azer::Vector4( 0.5f, -0.5f,  0.5f, 1.0f);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));
}

void AABBMesh::InitIndicesBuffer(RenderSystem* rs) {
  // create indices for face
  {
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

  // create for line
  {
    const int kEdgeNum = 12;
    IndicesDataPtr edge_idata(new IndicesData(kEdgeNum * 2, IndicesData::kUint32,
                                           IndicesData::kMainMemory));
    int32 edge_data[kEdgeNum * 2] = {
      0, 1, 1, 2, 2, 3, 3, 0,
      0, 4, 1, 5, 2, 6, 3, 7,
      4, 5, 5, 6, 6, 7, 7, 4,
    };
    int32* cur = (int32*)edge_idata->pointer();
    memcpy(cur, edge_data, sizeof(int32) * kEdgeNum * 2);
    edge_ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(),
                                           edge_idata));
  }
}


void AABBMesh::Render(Renderer* renderer) {
  /**
   * 根据 bbox 计算缩放的坐标进行缩放及移动
   * 以保证 AABB 的 Mesh 能够包裹住 box
   */
  renderer->DrawIndex(vb_.get(), edge_ib_.get(), kLineList);
  // renderer->Render(vb_.get(), ib_.get(), kLineList);
}

}  // namespace azer
