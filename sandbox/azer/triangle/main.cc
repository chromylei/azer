#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
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

  static const char* kVertexShader;
  static const char* kPixelShader;
  static const azer::VertexDesc::Desc kVertexDescs[];
};

void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();

  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs, 1));
  
  tech_.reset(rs->CreateEffect());
  vs_.reset(rs->CreateVertexGpuProgram(desc, kVertexShader));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, kPixelShader));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  azer::Vector3 v[] =
  {
    azer::Vector3( 0.0f, 0.5f, 0.5f ),
    azer::Vector3( -0.5f, -0.5f, 0.5f ),
    azer::Vector3( 0.5f, -0.5f, 0.5f ),
  };

  tech_->Use();
  data_.reset(new azer::VertexData(desc, ARRAYSIZE(v)));
  memcpy(data_->pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, 800, 600));
}

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"position", VertexDesc::kPosition, azer::kVec3},
};

const char* MainDelegate::kVertexShader = "\n"
    "float4 vs_main(float4 inPos : POSITION) : SV_POSITION \n"
    "{                                                \n"
    "  return inPos;                                  \n"
    "}";
const char* MainDelegate::kPixelShader = "\n"
    "float4 ps_main() : SV_TARGET                          \n"
    "{                                                \n"
    "  return float4(1.0f, 0.0f, 1.0f, 1.0f);         \n"
    "}";

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}
