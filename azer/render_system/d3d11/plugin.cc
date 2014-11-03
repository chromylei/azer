#include "azer/render_system/d3d11/plugin.h"
#include "azer/render_system/d3d11/render_system.h"

extern "C" {
azer::RenderSystem* CreateRenderSystem(azer::WindowHost* win) {
  std::unique_ptr<azer::D3D11RenderSystem> rs(new azer::D3D11RenderSystem(win));
  if (rs->Init()) {
    rs->GetDefaultRenderer()->SetViewport(azer::Renderer::Viewport());
    return rs.release();
  } else {
    return NULL;
  }
}
}  // extern "C"
