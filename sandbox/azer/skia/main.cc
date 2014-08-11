#include "sandbox/azer/rendertarget/delegate.h"

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");

  azer::RenderWindow::Options opt;
  opt.width = MainDelegate::kWindowWidth;
  opt.height = MainDelegate::kWindowHeight;
  MainDelegate delegate;
  azer::RenderWindow win(opt, &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}
