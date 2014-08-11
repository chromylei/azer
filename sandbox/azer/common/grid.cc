#include "sandbox/azer/common/grid.h"
#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "base/file_util.h"

#include "sandbox/azer/common/util.h"

using azer::VertexDesc;
const VertexDesc::Desc Grid::kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec3},
  {"TEXCOORD", VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL", VertexDesc::kTexCoord, azer::kVec3},
};

const int Grid::kDescNum = arraysize(kVertexDescs);

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/common/grid.vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/common/grid.ps.hlsl"));

void Grid::Init(const base::FilePath& filepath) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  CHECK(HeightMapLoad(filepath, &height_map_));
  InitVertex();
  InitTechnique();

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/grass.jpg"))));
}

void Grid::InitTechnique() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  std::string vertex_shader, fragment_shader;
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader));
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc(), vertex_shader));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  {
    azer::GpuConstantsTable::Desc desc[] = {
      azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
      azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4, 1),
    };
    vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(desc), desc));
  }

  {
    azer::GpuConstantsTable::Desc desc[] = {
      azer::GpuConstantsTable::Desc("light",
                                    azer::GpuConstantsType::kDirectionalLight, 1),
    };
    ps_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(desc), desc));
  }
}

void Grid::InitVertex() {
  const int kRow = height_map_.width;
  const int kColumn = height_map_.height;
  const int kVertexNum = kRow * kColumn;

  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));
  azer::VertexDataPtr data(new azer::VertexData(desc, kRow * kColumn));
  Vertex* begin_ptr = (Vertex*)data->pointer();
  Vertex* curr = begin_ptr;
  for (int i = 0; i < kColumn; ++i) {
    for (int j = 0; j < kRow; ++j) {
      int idx = i * kRow + j;
      curr->position = azer::vec3((float)i, height_map_.map[idx].y, (float)j);
      curr->tex = azer::Vector2(100.0f * j / (float)kRow,
                                100.0f * i / (float)kColumn);
      curr->normal = azer::vec3(0.0f, 1.0f, 0.0f);
      curr++;
    }
  }

  const int kTriangleNum = (kRow - 1) * (kColumn - 1) * 2;
  using azer::IndicesData;
  azer::IndicesDataPtr idata(new IndicesData(kTriangleNum * 3,
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
  int32* cur = (int32*)idata->pointer();
  for (int i = 0; i < kColumn-1; ++i) {
    for (int j = 0; j < kRow-1; ++j) {
      *cur++ = i * kRow + j;
      *cur++ = (i + 1) * kRow + j + 1;
      *cur++ = (i + 1) * kRow + j;
      
      *cur++ = i * kRow + j;
      *cur++ = i * kRow + j + 1;
      *cur++ = (i + 1) * kRow + j + 1;
    }
  }

  ::ComputeNormal((Vertex*)data->pointer(), kColumn * kRow,
                (DWORD*)idata->pointer(), kTriangleNum * 3);

  Vertex* v = (Vertex*)data->pointer();
  std::unique_ptr<NormalRender::Vertex[]>
      ndata(new NormalRender::Vertex[kVertexNum]);
  for (int i = 0; i < kVertexNum; ++i) {
    ndata.get()[i].position = azer::Vector4(v[i].position, 1.0);
    ndata.get()[i].normal = v[i].normal;
  }
  nrender_.Init(ndata.get(), kVertexNum);

  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));
}


void Grid::Render(azer::RenderSystem* rs, const azer::Matrix4& world,
                  Camera2* camera,  const azer::Light& light) {
  const int kRow = height_map_.width;
  const int kColumn = height_map_.height;
  const int kTriangleNum = (kRow - 1) * (kColumn - 1) * 2;
  azer::Matrix4 wvp = camera->projection() * camera->view() * world;

  // nrender_.Render(wvp);
  tech_->Use();

  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&world, sizeof(world));
  vs_table_->flush();

  ps_table_->SetLight(0, light);
  ps_table_->flush();

  rs->UseTexture(azer::kPixelStage, 0, tex_.get());
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());

  // rs->SetFillMode(azer::kWireFrame);
  // rs->SetCullingMode(azer::kCullBack);
  rs->SetCullingMode(azer::kCullNone);
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, kTriangleNum * 3);
}
