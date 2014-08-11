#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/loadobj.h"
#include "sandbox/azer/common/camera.h"
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

  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  ObjMeshLoader objloader_;
  azer::Mesh mesh_;
  azer::Light light_;
  Camera camera_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  XMMATRIX WVP;
  XMMATRIX meshworld;
  static const azer::VertexDesc::Desc kVertexDescs[];
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/bumpmap/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/bumpmap/ps.hlsl"));

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  
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

  CHECK(objloader_.Load(base::FilePath(TEXT("sandbox/azer/media/ground.obj")), &mesh_)) << "Failed to load obj";
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(mesh_.Subs()->at(0)->vb_ptr->desc(),
                                       vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  tech_->Finalize();

  InitLight();

  // objloader_.dump();
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

  // rs->SetFillMode(azer::kWireFrame);
  rs->SetCullingMode(azer::kCullNone);
  mesh_.Render(rs);
  rs->Present();
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

