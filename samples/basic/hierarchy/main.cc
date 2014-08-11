#include "azer/azer.h"

#include "azer/azer.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/strings/stringprintf.h"
#include "azer/util/util.h"
#include "samples/common/common.h"
#include "samples/common/camera_control.h"
#include "diffuse.h"
#include <tchar.h>

#define EFFECT_GEN_DIR FILE_PATH_LITERAL("out/dbg/gen/samples/basic/hierarchy/")
#define VS_SHADER_NAME FILE_PATH_LITERAL("diffuse.vs")
#define PS_SHADER_NAME FILE_PATH_LITERAL("diffuse.ps")

using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate
                   , public azer::SceneTraverser {
 public:
  MainDelegate()
      : culling_num_(0)
      , control_object_(0)
      , last_update_(0) {
  }

  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) OVERRIDE;
  virtual void OnRenderScene(double time, float delta_time) OVERRIDE;
  virtual void OnQuit() OVERRIDE {};

  // member of traverser
  virtual bool OnVisitBegin(azer::SceneNode* node) OVERRIDE;
  virtual void OnVisitEnd(azer::SceneNode* node) OVERRIDE {};
 private:
  int culling_num_;
  std::unique_ptr<DiffuseEffect> effect_;
  azer::Camera camera_;
  azer::ScenePtr scene_;

  azer::MaterialPtr base_mtrl_;
  std::unique_ptr<azer::RootWindow> root_ui_;
  azer::Label* cull_label_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
  std::unique_ptr<azer::ResourceManager> res_mgr_;
  int control_object_;
  double last_update_;
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
  
  azer::ShaderArray shaders;
  shaders.SetForStage(azer::kVertexStage, res_mgr_->GetGpuProgram(VS_SHADER_NAME));
  shaders.SetForStage(azer::kPixelStage, res_mgr_->GetGpuProgram(PS_SHADER_NAME));
  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));

  renderer->EnableDepthTest(true);
  renderer->SetCullingMode(azer::kCullNone);
  camera_.SetPosition(azer::Vector3(0.0f, 3.0f, 3.0f));
  scene_ = res_mgr_->GetScene(AZER_LITERAL("scene/hierarchy.sce:scene1"), rs);
  DCHECK(scene_.get() != NULL);

  base_mtrl_ = res_mgr_->GetMaterial(AZER_LITERAL("material/base.mtrl:base"), rs);
  grid_.reset(new azer::CoordinateGrid(rs, 100, 100, 100));

  root_ui_.reset(azer::RootWindow::Create(rs));
  {
    gfx::Rect rc(0, 0, 400, 300);
    gfx::Rect texrc(10, 20, 300, 30);
    azer::Window* win = azer::Window::Create(rc, root_ui_.get());
    cull_label_ = azer::Label::Create(texrc, win);
  }
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  azer::Radians camera_speed(azer::kPI / 2.0f);

  azer::SceneNodePtr joint1 = scene_->find(AZER_LITERAL("joint1"));
  azer::SceneNodePtr joint2 = scene_->find(AZER_LITERAL("joint2"));
  control_object_ = control_object_ % 3;
  if (control_object_ == 0) {
    UpdatedownCamera(&camera_, camera_speed, delta_time);
    cull_label_->SetText(::base::StringPrintf(AZER_LITERAL("Control Camera")));
  } else if (control_object_ == 1) {
    MovableControl(joint1.get(), camera_speed, delta_time);
    cull_label_->SetText(::base::StringPrintf(AZER_LITERAL("Control Joint1")));
  } else {
    MovableControl(joint2.get(), camera_speed, delta_time);
    cull_label_->SetText(::base::StringPrintf(AZER_LITERAL("Control Joint2")));
  }

  if (time - last_update_ > 0.2) {
    if( ::GetAsyncKeyState('M') & 0x8000f ) {
      last_update_ = time;
      control_object_++;
    }
  }

  // joint1->pitch(camera_speed * delta_time * 0.25f);
  // joint2->yaw(camera_speed * delta_time * 0.5f);
  root_ui_->Redraw();
  scene_->UpdateHierarchy();
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  azer::Renderer* renderer = rs->GetDefaultRenderer();

  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullNone);

  grid_->SetGridDiffuse(azer::Vector4(0.4f, 0.4f, 0.4f, 1.0f));
  grid_->SetProjViewMat(camera_.GetProjViewMatrix());
  grid_->Render(renderer);

  culling_num_ = 0;
  scene_->traverse(this);
  root_ui_->Render();
}

bool MainDelegate::OnVisitBegin(azer::SceneNode* node) {
  if (node->type() == azer::SceneNode::kMesh) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    const azer::Matrix4& world = node->GetWorldMatrix();
    azer::Matrix4 wvp = std::move(camera_.GetProjViewMatrix() * world);
    azer::MeshPtr mesh = node->GetAttachedMesh();
    if (!node->IsVisible(camera_.frustrum())) {
      culling_num_++;
      return false;
    }
    effect_->SetPVW(wvp);
    effect_->SetWorld(world);
    CHECK(mesh.get() != NULL) << node->node_name();
    /*
    for (auto iter = mesh->Subs().begin(); iter != mesh->Subs().end(); ++iter) {
      azer::Mesh::GroupVec& group = (*iter)->groups;
      group[0].mtrl_ptr = base_mtrl_;
    }
    */
    mesh->PrepareEffect(renderer, effect_.get());
    mesh->Render(renderer);
    mesh->RenderBoundingVolumn(renderer, world, camera_);
  }
  return true;
}

