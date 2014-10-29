#include "azer/render_system/d3d11/angle/grcontext.h"

#include "EGL/egl.h"
#include "gl/GrGLInterface.h"

#include "azer/render_system/d3d11/angle/angle.h"
#include "azer/render/render_system.h"
#include "base/logging.h"

namespace azer {
namespace skia {
SkAzerANGLEGrContext::SkAzerANGLEGrContext(int width, int height)
    : width_(width), height_(height) {
  DCHECK_GT(width_, 0);
  DCHECK_GT(height_, 0);
}

SkAzerANGLEGrContext::~SkAzerANGLEGrContext() {
  destroyGLContext();
}

const GrGLInterface* SkAzerANGLEGrContext::createGLContext() {
  RenderSystem* rs = RenderSystem::Current();
  context_.width = width_;
  context_.height = height_;
  if (!angle::Init(rs, &context_)) {
    return NULL;
  }

  SkAutoTUnref<const GrGLInterface> grinterface(GrGLCreateANGLEInterface());
  if (NULL == grinterface.get()) {
    LOG(ERROR) << "Failed to createANGLEInterface()";
    this->destroyGLContext();
    return NULL;
  }
  return GrGLInterfaceRemoveNVPR(grinterface.get());
}

void SkAzerANGLEGrContext::destroyGLContext() {
  angle::Destroy(&context_);
}

void SkAzerANGLEGrContext::makeCurrent() const {
  EGLContext context = (EGLContext)context_.context;
  EGLDisplay display = (EGLDisplay)context_.display;
  EGLSurface surface = (EGLSurface)context_.surface;
  if (!eglMakeCurrent(display, surface, surface, context)) {
    LOG(INFO) << "cannot set context!";
  }
}

void SkAzerANGLEGrContext::swapBuffers() const {
/*
  EGLContext context = (EGLContext)context_.context;
  EGLDisplay display = (EGLDisplay)context_.display;
  EGLSurface surface = (EGLSurface)context_.surface;
  if (!eglSwapBuffers(display, surface)) {
    LOG(INFO) << "cannot not complete eglSwwapBuffers!";
  }
*/
}
}  // namespace skia
}  // namespace azer
