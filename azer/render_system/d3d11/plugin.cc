#include "azer/render_system/d3d11/plugin.h"
#include "azer/render_system/d3d11/render_system.h"

extern "C" {
azer::RenderSystem* CreateRenderSystem(azer::WindowHost* win) {
  std::unique_ptr<azer::RenderSystem> rs(new azer::D3D11RenderSystem);
  if (rs->Init(win)) {
    rs->GetDefaultRenderer()->SetViewport(azer::Renderer::Viewport());
    return rs.release();
  } else {
    return NULL;
  }
}
}  // extern "C"
