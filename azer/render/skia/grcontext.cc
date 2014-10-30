#include "azer/render/skia/grcontext.h"

#include "gl/GrGLInterface.h"
#include "azer/render/render_system.h"
#include "azer/render/glcontext.h"
#include "base/logging.h"

namespace azer {
namespace skia {
AzerSkiaGrContext::AzerSkiaGrContext(int width, int height)
    : width_(width), height_(height) {
  DCHECK_GT(width_, 0);
  DCHECK_GT(height_, 0);
}

AzerSkiaGrContext::~AzerSkiaGrContext() {
  destroyGLContext();
}

const GrGLInterface* AzerSkiaGrContext::createGLContext() {
  RenderSystem* rs = RenderSystem::Current();
  context_.width = width_;
  context_.height = height_;
  agl_interface_ = rs->GetEGLInterface();
  if (!agl_interface_) {
    return NULL;
  }
  if (!agl_interface_->Init(&context_)) {
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

void AzerSkiaGrContext::destroyGLContext() {
  if (agl_interface_) {
    agl_interface_->Destroy(&context_);
  }
}

void AzerSkiaGrContext::makeCurrent() const {
  DCHECK(agl_interface_ != NULL);
  if (!agl_interface_->MakeCurrent(&context_)) {
    LOG(INFO) << "cannot set context!";
  }
}

void AzerSkiaGrContext::swapBuffers() const {
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
