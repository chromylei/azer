#include "azer/azer.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "azer/util/util.h"
#include "samples/common/common.h"
#include "samples/common/camera_control.h"
#include "diffuse.h"
#include <tchar.h>

#define EFFECT_GEN_DIR FILE_PATH_LITERAL("out/dbg/gen/samples/basic/mesh/")
#define VS_SHADER_NAME FILE_PATH_LITERAL("diffuse.vs")
#define PS_SHADER_NAME FILE_PATH_LITERAL("diffuse.ps")

using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) OVERRIDE;
  virtual void OnRenderScene(double time, float delta_time) OVERRIDE;
  virtual void OnQuit() OVERRIDE {};
 private:
  std::unique_ptr<DiffuseEffect> effect_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
  azer::Matrix4 world_;
  azer::Camera camera_;
  azer::MeshPtr mesh_ptr_;
  std::unique_ptr<azer::ResourceManager> res_mgr_;
};

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  ::azer::FileSystem::InitDefaultFileSystem(AZER_LITERAL("azer/resources/common"));
  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}

void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();

  azer::FileSystemPtr sample_res_fs;
  azer::FileSystemPtr shader_gen_fs;
  sample_res_fs.reset(azer::FileSystem::create(
      azer::FileSystem::kNativeFS, ::base::FilePath(SAMPLE_RES_DIR)));
  shader_gen_fs.reset(azer::FileSystem::create(
      azer::FileSystem::kNativeFS, ::base::FilePath(EFFECT_GEN_DIR)));
  res_mgr_.reset(new azer::ResourceManager(sample_res_fs, shader_gen_fs));
  mesh_ptr_ = res_mgr_->GetMesh(AZER_LITERAL("mesh/sphere.afxm"), rs);
  
  azer::ShaderArray shaders;
  shaders.SetForStage(azer::kVertexStage, res_mgr_->GetGpuProgram(VS_SHADER_NAME));
  shaders.SetForStage(azer::kPixelStage, res_mgr_->GetGpuProgram(PS_SHADER_NAME));
  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));

  renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  renderer->EnableDepthTest(true);
  renderer->SetCullingMode(azer::kCullNone);
  camera_.SetPosition(azer::Vector3(0.0f, 3.0f, 0.0f));

  grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  world_ = azer::Translate(0.0, 0.0, -8.0)
      * azer::RotateY(azer::Radians(time * rspeed));

  azer::Radians camera_speed(azer::kPI / 2.0f);
  UpdatedownCamera(&camera_, camera_speed, delta_time);
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  azer::Renderer* renderer = rs->GetDefaultRenderer();
    
  azer::Matrix4 wvp = camera_.GetProjViewMatrix() * world_;
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullNone);
  effect_->SetPVW(wvp);
  effect_->SetWorld(world_);
  mesh_ptr_->PrepareEffect(renderer, effect_.get());
  mesh_ptr_->Render(renderer);

  mesh_ptr_->RenderBoundingVolumn(renderer, world_, camera_);

  grid_->SetProjViewMat(camera_.GetProjViewMatrix());
  grid_->SetGridDiffuse(azer::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
  grid_->Render(renderer);
}
