#include "azer/render_system/d3d11/angle/glinterface.h"


#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "azer/render_system/d3d11/angle/angle.h"


namespace azer {
bool ANGLEGLInterface::Init(AzerEGLContext* context) {
  RenderSystem* rs = RenderSystem::Current();
  if (! angle::Init(rs, context)) {
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

Texture* ANGLEGLInterface::CreateTexture(uint32 gltexid) {
  return NULL;
}
}  // namespace
