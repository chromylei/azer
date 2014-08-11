#include "azer/render/render.h"
#include "sandbox/azer/common/config.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/loadobj.h"
#include "sandbox/azer/common/camera2.h"
#include "sandbox/azer/common/skysphere.h"
#include "sandbox/azer/common/billboard.h"
#include <tchar.h>

const int kWindowWidth = 800;
const int kWindowHeight = 600;

azer::WindowHost *gMainwin = NULL;

struct InstanceData {
  azer::Vector3 pos;
};


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
  void RenderGround(azer::RenderSystem* rs);

  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  std::string vertex_shader_;
  std::string fragment_shader_;
  
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::Mesh mesh_;
  azer::Light light_;
  Camera2 camera_;
  SkySphere sphere_;

  float last_click_;

  azer::Matrix4 meshworld;

  azer::TexturePtr tex_;
  Billboard billboard_;
  static const azer::VertexDesc::Desc kVertexDescs[];
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/picking/ground.vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/picking/ground.ps.hlsl"));

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight)
    , last_click_(0.0f) {
}

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));

  azer::RenderSystem* rs = azer::RenderSystem::Current();
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));
  azer::GpuConstantsTable::Desc dvs_desc[] = {
    azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
    azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(dvs_desc), dvs_desc));

  azer::GpuConstantsTable::Desc pvs_desc[] = {
    azer::GpuConstantsTable::Desc("light",
                                  azer::GpuConstantsType::kDirectionalLight, 1),
  };
  ps_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(pvs_desc), pvs_desc));
  
  ObjMeshLoader objloader1, objloader2;
  CHECK(objloader1.Load(base::FilePath(TEXT("sandbox/azer/media/ground.obj")),
                        &mesh_)) << "Failed to load obj";

  vs_.reset(rs->CreateVertexGpuProgram(mesh_.Subs()->at(0)->vb_ptr->desc(),
                                       vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_.reset(rs->CreateEffect());
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->Finalize();
  
  InitLight();
  sphere_.Init();
  // objloader_.dump();

  std::vector<azer::Vector3> pos;
  pos.push_back(azer::Vector3(0.0f, 1.0f, 0.0f));
  billboard_.Init(pos);

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/treeBillTexture.png"))));
  camera_.SetPosition(azer::Vector3(0.0f, 3.0f, 15.0f));
  camera_.SetTarget(azer::Vector3(0.0f, 3.0f, 0.0f));
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  camera_.OnInput();
  meshworld = azer::Transpose( 0.0f, 0.0f, 0.0f );
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  sphere_.Render(camera_);

  RenderGround(rs);

  azer::Matrix4 scale = azer::Scale(5.0f, 5.0f, 5.0f);
  billboard_.Render(tex_.get(), camera_, scale);
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, 0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void MainDelegate::RenderGround(azer::RenderSystem* rs) {
  azer::Matrix4 proj = camera_.projection();
  azer::Matrix4 wvp =  proj * camera_.view() * meshworld;
  tech_->Use();
  vs_table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  vs_table_->SetValue(1, (void*)&meshworld, sizeof(meshworld));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage,vs_table_.get());
  ps_table_->SetLight(0, light_);
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  ps_table_->flush();

  rs->SetFillMode(azer::kSolid);
  rs->SetCullingMode(azer::kCullNone);
  mesh_.Render(rs);
}

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  

  azer::WindowHost::Options opt;
  opt.width = kWindowWidth;
  opt.height = kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
  gMainwin = &win;
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(
      ::base::FilePath(RENDER_SYSTEM_DLL), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}

