#include "azer/render_system/d3d11/angle/glinterface.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "azer/render_system/d3d11/angle/angle.h"
#include "azer/render_system/d3d11/angle/texture.h"


namespace azer {
bool ANGLEGLInterface::Init(AzerEGLContext* context) {
  RenderSystem* rs = RenderSystem::Current();
  return angle::Init(rs, context);
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

AzerGLTexture* ANGLEGLInterface::CreateTexture(int width, int height) {
  std::unique_ptr<angle::AngleTexture> ptr(new angle::AngleTexture(width, height));
  if (ptr->Init()) {
    return ptr.release();;
  }

  return NULL;
}
}  // namespace
