#include "sandbox/azer/skybox/delegate.h"

struct Vertex {
  azer::Vector3 position;
  azer::Vector2 tex;
  azer::Vector3 normal;

  Vertex(const azer::Vector3& p, const azer::Vector2& t, const azer::Vector3& n)
      : position(p), tex(t), normal(n) {
  }
};

//Create the vertex buffer
Vertex v[] = {
  // Bottom Face
  Vertex(azer::Vector3(-1.0f, -1.0f, -1.0f),
         azer::Vector2(100.0f, 100.0f),
         azer::Vector3(0.0f, 1.0f, 0.0f)),
  Vertex(azer::Vector3( 1.0f, -1.0f, -1.0f),
         azer::Vector2(0.0f, 100.0f),
         azer::Vector3(0.0f, 1.0f, 0.0f)),
  Vertex(azer::Vector3( 1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f,   0.0f),
         azer::Vector3(0.0f, 1.0f, 0.0f)),
  Vertex(azer::Vector3(-1.0f, -1.0f,  1.0f),
         azer::Vector2(100.0f,   0.0f),
         azer::Vector3(0.0f, 1.0f, 0.0f)),
};

DWORD indices[] = {
  0,  1,  2,
  0,  2,  3,
};

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec3},
  {"TEXCOORD", VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL", VertexDesc::kTexCoord, azer::kVec3},
};

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/fpcamera/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/fpcamera/ps.hlsl"));
void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/grass.jpg"))));
  InitVertexAndIndices();
  InitConstantTable();
  InitLight();
  sphere_.Init();

  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  camera_.SetPosition(azer::Vector3(0.0f, 2.0f, 8.0f));
}

void MainDelegate::InitVertexAndIndices() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));
  
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc, vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  data_.reset(new azer::VertexData(desc, ARRAYSIZE(v)));
  memcpy(data_->pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  tech_->Finalize();

  using azer::IndicesData;
  azer::IndicesDataPtr idata(new IndicesData(arraysize(indices),
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
  memcpy(idata->pointer(), (uint8*)&indices, sizeof(indices));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));
}

void MainDelegate::InitConstantTable() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
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

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  azer::Matrix4 scale = azer::Scale(500.0f, 10.0f, 500.0f);
  azer::Matrix4 trans = azer::Transpose(0.0f, 10.0f, 0.0f);
  groundWorld =  trans * scale;

  camera_.OnInput();
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  sphere_.Render(camera_);

  ps_table_->SetLight(0, light_);
  ps_table_->flush();
  rs->UseTexture(azer::kPixelStage, 0, tex_.get());
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());

  azer::Matrix4 proj = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                              4.0f / 3.0f,
                                              1.0f, 1000.0f);
  azer::Matrix4 wvp = proj * camera_.view() * groundWorld;
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&groundWorld, sizeof(groundWorld));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());

  rs->SetFillMode(azer::kSolid);
  rs->SetCullingMode(azer::kCullFront);
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, arraysize(indices), 0);
  
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, 0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

