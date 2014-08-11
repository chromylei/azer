#include "azer/render/render.h"
#include "sandbox/azer/common/config.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/at_exit.h"
#include "base/file_util.h"
#include <tchar.h>

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    DCHECK(NULL != rs);
    rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    rs->ClearDepthAndStencil();
    tech_->Use();
    rs->Render(vb_.get(), azer::kTriangleList, 3, 0);
    rs->Present();
  }

  virtual void OnQuit() {}
 private:
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  static const azer::VertexDesc::Desc kVertexDescs[];
};

struct Vertex {
  azer::Vector4 position;
  azer::Vector4 color;

  Vertex(const azer::Vector4& p, const azer::Vector4& c)
      : position(p), color(c) {
  }
};

Vertex v[] = {
  Vertex(azer::Vector4(0.5f, -0.5f, 0.5f, 1.0f),
         azer::Vector4(0.0f, 1.0f, 0.0f, 1.0f)),
    
  Vertex(azer::Vector4(0.0f, 0.5f, 0.5f, 1.0f),
         azer::Vector4(1.0f, 0.0f, 0.0f, 1.0f)),

  Vertex(azer::Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
         azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f)), 
  
};

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"position", VertexDesc::kPosition, azer::kVec4},
  {"color", VertexDesc::kDiffuse, azer::kVec4},
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/colored/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/colored/ps.hlsl"));

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  
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

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, 800, 600));
}

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
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
