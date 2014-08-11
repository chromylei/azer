#include "sandbox/ui/widget/delegate.h"
#include "sandbox/azer/common/config.h"

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");

  azer::WindowHost::Options opt;
  opt.width = MainDelegate::kWindowWidth;
  opt.height = MainDelegate::kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
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
