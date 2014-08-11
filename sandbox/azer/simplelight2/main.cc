#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
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
  void InitLight();
  void InitCamera();
  void InitConstantTable();
  void InitVertexAndIndices();
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

  NormalRender nrender_;

  azer::Matrix4 camView;
  azer::Matrix4 camProjection;

  azer::Matrix4 cube1World;
  azer::Matrix4 cube2World;

  static const azer::VertexDesc::Desc kVertexDescs[];
};

struct Vertex {
  azer::Vector4 position;
  azer::Vector2 tex;
  azer::Vector3 normal;

  Vertex(const azer::Vector4& p, const azer::Vector2& t, const azer::Vector3& n)
      : position(p), tex(t) , normal(n) {
  }
};


Vertex v[] = {
  // Front Face
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(-1.0f, -1.0f, -1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(-1.0f,  1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(1.0f,  1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(1.0f, -1.0f, -1.0f)),

  // Back Face
  Vertex(azer::Vector4(-1.0f, -1.0f, 1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(-1.0f, -1.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, 1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(1.0f, -1.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, 1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(1.0f,  1.0f, 1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, 1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(-1.0f,  1.0f, 1.0f)),

  // Top Face
  Vertex(azer::Vector4(-1.0f, 1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(-1.0f, 1.0f, -1.0f)),
  Vertex(azer::Vector4(-1.0f, 1.0f,  1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(-1.0f, 1.0f,  1.0f)),
  Vertex(azer::Vector4( 1.0f, 1.0f,  1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(1.0f, 1.0f,  1.0f)),
  Vertex(azer::Vector4( 1.0f, 1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(1.0f, 1.0f, -1.0f)),

  // Bottom Face
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(-1.0f, -1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(1.0f, -1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f,  1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(1.0f, -1.0f,  1.0f)),
  Vertex(azer::Vector4(-1.0f, -1.0f,  1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(-1.0f, -1.0f,  1.0f)),

  // Left Face
  Vertex(azer::Vector4(-1.0f, -1.0f,  1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(-1.0f, -1.0f,  1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f,  1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(-1.0f,  1.0f,  1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(-1.0f,  1.0f, -1.0f)),
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(-1.0f, -1.0f, -1.0f)),

  // Right Face
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 1.0f),
         azer::Vector3(1.0f, -1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, -1.0f,  1.0f),
         azer::Vector2(0.0f, 0.0f),
         azer::Vector3(1.0f,  1.0f, -1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f,  1.0f,  1.0f),
         azer::Vector2(1.0f, 0.0f),
         azer::Vector3(1.0f,  1.0f,  1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f,  1.0f,  1.0f),
         azer::Vector2(1.0f, 1.0f),
         azer::Vector3(1.0f, -1.0f,  1.0f)),
};

DWORD indices[] = {
  // Front Face
  0,  1,  2,
  0,  2,  3,

  // Back Face
  4,  5,  6,
  4,  6,  7,

  // Top Face
  8,  9, 10,
  8, 10, 11,

  // Bottom Face
  12, 13, 14,
  12, 14, 15,

  // Left Face
  16, 17, 18,
  16, 18, 19,

  // Right Face
  20, 21, 22,
  20, 22, 23
};

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec4},
  {"TEXCOORD", VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL", VertexDesc::kTexCoord, azer::kVec3},
};

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/simplelight2/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/simplelight2/ps.hlsl"));
void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/braynzar.jpg"))));

  InitVertexAndIndices();
  InitConstantTable();
  InitLight();
  InitCamera();

  std::unique_ptr<NormalRender::Vertex[]> ndata(new NormalRender::Vertex[arraysize(v)]);
  for (int i = 0; i < arraysize(v); ++i) {
    ndata.get()[i].position = v[i].position;
    ndata.get()[i].normal = v[i].normal;
  }
  nrender_.Init(ndata.get(), arraysize(v));
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

  ps_table_->SetLight(0, light_);
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  ps_table_->flush();

  azer::Matrix4 wvp =  camProjection * camView * cube1World;
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&cube1World, sizeof(cube1World));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, 36, 0);
  nrender_.Render(wvp);

  wvp = camProjection * camView * cube2World;
  tech_->Use();
  rs->UseTexture(azer::kPixelStage, 0, tex_.get());
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&cube2World, sizeof(cube2World));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, 36, 0);
  nrender_.Render(wvp);
  
  rs->Present();
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

