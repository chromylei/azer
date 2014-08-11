#include "sandbox/azer/common/skysphere.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "azer/math/math.h"
#include "azer/math/degree.h"

#include <iostream>


::base::FilePath skybox_vertex_shader_path(TEXT("sandbox/azer/common/sky.vs.hlsl"));
::base::FilePath skybox_pixel_shader_path(TEXT("sandbox/azer/common/sky.ps.hlsl"));

using azer::VertexDesc;
const VertexDesc::Desc kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec3},
};

void SkySphere::Init() {
  InitVertex();
  InitIndices();

  azer::RenderSystem* rs = azer::RenderSystem::Current();

  CHECK(::base::ReadFileToString(skybox_vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(skybox_pixel_shader_path, &fragment_shader_));

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::kCubeMap, base::FilePath(TEXT("sandbox/azer/media/skymap.dds"))));

  
  {
    azer::GpuConstantsTable::Desc desc[] = {
      azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
      azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4, 1),
    };
    vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(desc), desc));
  }

  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));
  
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc, vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  data_.reset(new azer::VertexData(desc, kVertexNum));
  memcpy(data_->pointer(), (uint8*)vertices_.get(), sizeof(Vertex) * kVertexNum);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));

  using azer::IndicesData;
  azer::IndicesDataPtr idata(new IndicesData(kIndicesNum,
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
  memcpy(idata->pointer(), (uint8*)indices_.get(), sizeof(uint32) * kIndicesNum);
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));
}

void SkySphere::InitIndices() {
  indices_.reset(new uint32[kIndicesNum]);
  int index = 0;
  for (int lat = 0; lat < kLatLines - 3; ++lat) {
    for (int lon = 0; lon < kLongLines; ++lon) {
      indices_[index++] = 1 + lat * kLongLines + lon;
      indices_[index++] = 1 + lat * kLongLines + (lon + 1) % kLongLines;
      indices_[index++] = 1 + (lat + 1) * kLongLines + lon;
    
      indices_[index++] = 1 + lat * kLongLines + (lon + 1) % kLongLines;
      indices_[index++] = 1 + (lat + 1) * kLongLines + (lon + 1) % kLongLines;
      indices_[index++] = 1 + (lat + 1) * kLongLines + lon;
    }
  }

  for (int lon = 0; lon < kLongLines; ++lon) {
    int base = 1;
    indices_[index++] = 0;
    indices_[index++] = base + (lon + 1) % kLongLines;
    indices_[index++] = base + lon;
  }

  for (int lon = 0; lon < kLongLines; ++lon) {
    indices_[index++] = kVertexNum - 1;
    indices_[index++] = kVertexNum - 1 - kLongLines + lon;
    indices_[index++] = kVertexNum - 1 - kLongLines + (lon + 1) % kLongLines;
  }

  DCHECK(index == kIndicesNum);
}

void SkySphere::InitVertex() {
  vertices_.reset(new Vertex[kVertexNum]);
  
  int index = 0;
  vertices_[index++].position = azer::Vector3(0.0f, 1.0f, 0.0f);
  
  for (int lat = 1; lat < kLatLines - 1; ++lat) {
    azer::Radians yRadians = azer::kPI * (float(kLatLines - lat) / (float)kLatLines)
        - azer::kPI / 2.0f;
    float radius = azer::cos(yRadians);
    float y = azer::sin((yRadians));
    for (int lon = 0; lon < kLongLines; ++lon) {
      azer::Radians xzRadians = azer::kPI * 2.0 * ((float)(kLongLines - lon)
                                                   / (float)kLongLines);
      vertices_[index++].position = azer::Vector3(radius * azer::sin(xzRadians),
                                                  y,
                                                  radius * azer::cos(xzRadians));
    }
  }

  vertices_[index++].position = azer::Vector3(0.0f, -1.0f, 0.0f);
  DCHECK(index == kVertexNum);
}


void SkySphere::Render(const Camera2& camera) {
  
  float scale_factor = 50.0;
  azer::Matrix4 scale = azer::Scale(scale_factor, scale_factor, scale_factor);
  azer::Vector3 pos = camera.position();
  
  azer::Matrix4 world = azer::Transpose(pos) * scale;
  // world = Scale;
  azer::Matrix4 wvp =  camera.projection() * camera.view() * world;
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::ScopedDepthBufferState scoped_depth(rs->CurrentDepthBuffer());
  rs->CurrentDepthBuffer()->SetDepthCompareFunc(azer::DepthBuffer::kLessEqual);
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&world, sizeof(world));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->UseTexture(azer::kPixelStage, 0, tex_.get());

  rs->SetFillMode(azer::kSolid);
  rs->SetCullingMode(azer::kCullNone);

  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, kIndicesNum);
}
