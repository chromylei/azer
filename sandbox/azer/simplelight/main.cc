#include "base/basictypes.h"
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/simplelight/data.h"
#include "sandbox/azer/common/normal_render.h"
#include <tchar.h>

const int kWindowWidth = 800;
const int kWindowHeight = 600;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate();
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);

  virtual void OnQuit() {}
 private:
  void InitShader(azer::VertexDescPtr desc);
  void InitVertex();
  void InitLight();
  void InitCamera();
  
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::TexturePtr tex_;
  azer::Light light_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  azer::Matrix4 camView;
  azer::Matrix4 camProjection;

  azer::Matrix4 cube1World;
  azer::Matrix4 cube2World;

  NormalRender nrender_;
  static const azer::VertexDesc::Desc kVertexDescs[];
};

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec4},
  {"TEXCOORD", VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL",   VertexDesc::kNormal,   azer::kVec3},
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/simplelight/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/simplelight/ps.hlsl"));

void ComputeNormal(Vertex* v, int vertex_num, uint32* indices, int indices_num) {
  for (int i = 0; i < indices_num; i+=3) {
    Vertex* p1 = v + indices[i];
    Vertex* p2 = v +indices[i+1];
    Vertex* p3 = v + indices[i+2];
    azer::Vector3 normal = azer::CalcPlaneNormal(p1->position,
                                                 p2->position,
                                                 p3->position);
    p1->normal += normal;
    p2->normal += normal;
    p3->normal += normal;
  }

  for (int i = 0; i < vertex_num; ++i) {
    v[i].normal /= 3.0f;
  }
}

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));

  ComputeNormal(v, kVertexNum, indices, kIndicesNum);
  std::unique_ptr<NormalRender::Vertex[]> nrender_data(new NormalRender::Vertex[kVertexNum]);
  for (int i = 0; i < kVertexNum; ++i) {
    nrender_data.get()[i].position = v[i].position;
    nrender_data.get()[i].normal = v[i].normal;
  }
  nrender_.Init(nrender_data.get(), kVertexNum);
  azer::RenderSystem* rs = azer::RenderSystem::Current();

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

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

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/braynzar.jpg"))));

  InitVertex();
  InitLight();
  InitCamera();
}

void MainDelegate::InitCamera() {
  //Camera information
  azer::Vector3 camPosition( 0.0f, 3.0f, -8.0f);
  azer::Vector3 camTarget( 0.0f, 0.0f, 0.0f);
  azer::Vector3 camUp( 0.0f, 1.0f, 0.0f);

  //Set the View matrix
  camView = LookAtRH(camPosition, camTarget, camUp );

  //Set the Projection matrix
  camProjection = PerspectiveRHD3D(azer::Radians(3.14f / 4.0f),
                                   (float)kWindowWidth / (float)kWindowHeight,
                                   1.0f, 1000.0f);
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, 0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void MainDelegate::InitShader(azer::VertexDescPtr desc) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc, vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage,
                                 fragment_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->Use();
  tech_->Finalize();  
}

void MainDelegate::InitVertex() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));
  data_.reset(new azer::VertexData(desc, kVertexNum));
  memcpy(data_->pointer(), (uint8*)v, sizeof(Vertex) * kVertexNum);
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  using azer::IndicesData;
  azer::IndicesDataPtr idata(new IndicesData(kIndicesNum,
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
  memcpy(idata->pointer(), (uint8*)&indices, kIndicesNum * sizeof(DWORD));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));

  InitShader(desc);
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  azer::Matrix4 Rotation = azer::RotateY(azer::Radians(time));
  azer::Matrix4 Translation = azer::Transpose( 0.0f, 0.0f, 4.0f );
  cube1World = Rotation * Translation;
  
  cube2World = azer::Matrix4::kIdentity;
  Rotation = azer::RotateY(azer::Radians(-time));
  azer::Matrix4 Scale = azer::Scale(1.3f, 1.3f, 1.3f);
  Translation = azer::Transpose(0.0f, 0.0f, 4.0f);
  cube2World = Rotation * Scale;
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  ps_table_->SetLight(0, light_);
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  ps_table_->flush();

  azer::Matrix4 wvp =  camProjection * camView * cube1World;
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&cube1World, sizeof(cube1World));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  // rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, kIndicesNum, 0);
  nrender_.Render(wvp);

  wvp = camProjection * camView * cube2World;
  tech_->Use();
  rs->UseTexture(azer::kPixelStage, 0, tex_.get());
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&cube2World, sizeof(cube2World));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  // rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, kIndicesNum, 0);
  nrender_.Render(wvp);
  
  rs->Present();
}

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  

  azer::WindowHost::Options opt;
  opt.width = kWindowWidth;
  opt.height = kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}

