#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/loadobj.h"
#include "sandbox/azer/common/camera.h"
#include "sandbox/azer/common/skysphere.h"
#include "sandbox/azer/instance/tree_mesh.h"

#include <tchar.h>
#include <xnamath.h>

const int kWindowWidth = 800;
const int kWindowHeight = 600;

azer::WindowHost *gMainwin = NULL;

struct InstanceData {
  XMFLOAT3 pos;
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
  Camera camera_;
  SkySphere sphere_;

  float last_click_;
  XMMATRIX WVP;
  XMMATRIX meshworld;

  TreeMesh tree_;

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
  tree_.Init();
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  camera_.OnInput();
  XMMATRIX Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );
  meshworld = Translation;
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
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, 0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void MainDelegate::RenderGround(azer::RenderSystem* rs) {
  WVP = meshworld * camera_.view() * camera_.projection();
  XMMATRIX twvp = XMMatrixTranspose(WVP);
  XMMATRIX tworld = XMMatrixTranspose(meshworld);
  tech_->Use();
  vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
  vs_table_->SetValue(1, (void*)&tworld, sizeof(tworld));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage,vs_table_.get());
  ps_table_->SetLight(0, light_);
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  ps_table_->flush();

  rs->SetFillMode(azer::kSolid);
  rs->SetCullingMode(azer::kCullNone);
  mesh_.Render(rs);

  tree_.Render(camera_, light_);
}

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  

  azer::WindowHost::Options opt;
  opt.width = kWindowWidth;
  opt.height = kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
  gMainwin = &win;
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}

