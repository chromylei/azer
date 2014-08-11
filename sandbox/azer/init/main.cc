#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include <tchar.h>

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* renderer = azer::RenderSystem::Current();
    DCHECK(NULL != renderer);
    renderer->Clear(azer::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    renderer->Present();
  }

  virtual void OnQuit() {}
 private:
};

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << azer::RenderSystem::Current()->name();
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}
