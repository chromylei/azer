#include "azer/render_system/d3d11/angle/glinterface.h"


#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "libGLESv2/main.h"
#include "azer/render_system/d3d11/angle/angle.h"
#include "azer/render_system/d3d11/texture.h"


namespace azer {
bool ANGLEGLInterface::Init(AzerEGLContext* context) {
  if (! angle::Init((RenderSystem*)render_system_, context)) {
    return false;
  }
  return true;
}

bool ANGLEGLInterface::MakeCurrent(const AzerEGLContext* ctx) {
  EGLContext context = (EGLContext)ctx->context;
  EGLDisplay display = (EGLDisplay)ctx->display;
  EGLSurface surface = (EGLSurface)ctx->surface;
  return eglMakeCurrent(display, surface, surface, context);
}

void ANGLEGLInterface::Destroy(AzerEGLContext* context) {
  angle::Destroy(context);
}

Texture* ANGLEGLInterface::GetShareTextureFromTex(uint32 texid) {
  D3D11RenderSystem* rs = render_system_;
  HANDLE handle = 0;
  glGetTexShareD3DTex(GL_DRAW_FRAMEBUFFER_ANGLE, texid, &handle);
  return D3D11Texture2DExtern::Create(handle, rs);
}
}  // namespace
