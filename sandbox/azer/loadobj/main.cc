#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/loadobj.h"
#include <tchar.h>

const int kWindowWidth = 800;
const int kWindowHeight = 600;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);

  virtual void OnQuit() {}
 private:
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::GpuConstantsTablePtr table_;
  ObjMeshLoader objloader_;
  azer::Mesh mesh_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  azer::Matrix4 camView;
  azer::Matrix4 camProjection;

  azer::Matrix4 cube1World;
  azer::Matrix4 cube2World;

  static const azer::VertexDesc::Desc kVertexDescs[];
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/loadobj/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/loadobj/ps.hlsl"));

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

  azer::Vector3 camPosition( 0.0f, 3.0f, -8.0f);
  azer::Vector3 camTarget( 0.0f, 0.0f, 0.0f);
  azer::Vector3 camUp( 0.0f, 1.0f, 0.0f);

  //Set the View matrix
  camView = LookAtRH(camPosition, camTarget, camUp );

  //Set the Projection matrix
  camProjection = PerspectiveRHD3D(azer::Radians(3.14f / 4.0f),
                                   (float)kWindowWidth / (float)kWindowHeight,
                                   1.0f, 1000.0f);
  {
    azer::GpuConstantsTable::Desc desc[] = {
      azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
    };
    table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(desc), desc));
  }

  objloader_.Load(base::FilePath(TEXT("sandbox/azer/media/spaceCompound.obj")), &mesh_);
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(mesh_.Subs()->at(0)->vb_ptr->desc(),
                                       vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  tech_->Finalize();

  // objloader_.dump();
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  cube1World = azer::Transpose( 0.0f, 0.0f, 4.0f )
      * azer::RotateY(azer::Degree(time));
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));

  azer::Matrix4 wvp =  camProjection * camView * cube1World;
  tech_->Use();
  table_->SetValue(0, (void*)&wvp, sizeof(wvp));
  table_->flush();
  rs->UseConstantsTable(azer::kVertexStage,table_.get());

  // rs->SetFillMode(azer::kWireFrame);
  rs->SetCullingMode(azer::kCullNone);
  mesh_.Render(rs);
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

