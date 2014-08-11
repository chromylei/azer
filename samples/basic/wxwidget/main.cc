#include <tchar.h>

#include "samples/basic/wxwidget/azer_canvas.h"
#include "azer/azer.h"
#include "base/base.h"
#include "diffuse.h"

#define EFFECT_GEN_DIR "out/dbg/gen/samples/basic/wxwidget/"
#define SHADER_NAME "diffuse"
using base::FilePath;

// Define a new application type
class MyApp: public wxApp {
 public:
  bool OnInit();
};

class MyDelegate : public AzerCanvas::Delegate {
 public:
  virtual void OnCreate(HWND hwnd);
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 protected:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
};

bool MyApp::OnInit() {
  MyDelegate* delegate = new MyDelegate;
  AzerFrame *frame = new AzerFrame(_T("Azer Widget"), wxPoint(50,50),
                                   wxSize(800,600), delegate);
  frame->Show(TRUE);
  SetTopWindow(frame);
  return TRUE;
}

void MyDelegate::OnCreate(HWND hwnd) {
  azer::WindowHost::Options opt;
  azer::WindowHost* win = new azer::WindowHost(opt);
  win->Attach((NativeWindowHandle)hwnd);
  CHECK(azer::LoadRenderSystem(win));
  
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));


  effect_.reset(new DiffuseEffect(shaders.GetShaderVec(),
                                  azer::RenderSystem::Current()));

  azer::Vector4 v[] = {
    azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
    azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
    azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
  };
  data_.reset(new azer::VertexData(effect_->GetVertexDesc(), ARRAYSIZE(v)));
  memcpy(data_->pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  CHECK(renderer->GetFrontFace() == azer::kCounterClockwise);
  CHECK(renderer->GetCullingMode() == azer::kCullBack);
}

void MyDelegate::OnUpdateScene(double time, float delta_time) {
}

void MyDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  DCHECK(NULL != rs);
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  effect_->SetDiffuse(azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  effect_->Use(renderer);
  renderer->Render(vb_.get(), azer::kTriangleList, 3, 0);
  rs->Present();
}

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  wxApp* pApp = new MyApp(); 
  wxApp::SetInstance(pApp);
  wxEntry(argc, argv);
//  wxEntryStart(argc, argv);
// wxTheApp->CallOnInit();
  wxEntryCleanup();
  return 0;
}
