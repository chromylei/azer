#include "azer/render/skia/context.h"

#include "gl/GrGLFunctions.h"
#include "gl/GrGLDefines.h"
#include "gl/GrGLUtil.h"
#include "gl/GrGLInterface.h"
#include "gl/SkGLContextHelper.h"
#include "SkCanvas.h"
#include "SkGpuDevice.h"
#include "GrTexture.h"
#include "GrContext.h"
#include "SkImageInfo.h"
#include "SkImageEncoder.h"
#include "azer/render/skia/grcontext.h"

namespace azer {
namespace skia {

// class Context
Context::Context()
    : gr_context_(NULL)
    , interface_(NULL)
    , helper_(NULL) {
}

Context::~Context() {
  if (gr_context_) {
    delete gr_context_;
  }

  if (helper_) {
    delete helper_;
  }
}

bool Context::Init() {
  // code reference: skia/include/gpu/GrContextFactory.h
  helper_ = new AzerSkiaGrContext(300, 300);
  SkGLContextHelper* glctx = helper_;
  static const int kBogusSize = 1;
  if (!glctx->init(kBogusSize, kBogusSize)) {
    LOG(ERROR) << "Failed to init GrContext";
    return false;
  }

  interface_ = glctx->gl();
  interface_ = GrGLInterfaceRemoveNVPR(interface_);
  if (!interface_) {
    return false;
  }

  glctx->makeCurrent();
  GrBackendContext p3dctx = reinterpret_cast<GrBackendContext>(interface_);
  gr_context_ = GrContext::Create(kOpenGL_GrBackend, p3dctx);
  if (!gr_context_) {
    return false;
  }

  return true;
}

CanvasPtr Context::CreateCanvas(int width, int height) {
  std::unique_ptr<Canvas> ptr(new Canvas(width, height, this));
  if (ptr->Init()) {
    return CanvasPtr(ptr.release());
  } else {
    return CanvasPtr();
  }
}

AzerEGLContext* Context::GetAzerEGLContext() {
  DCHECK(helper_ != NULL);
  return helper_->GetAzerEGLContext();
}

AzerEGLInterface* Context::GetAzerEGLInterface() {
  DCHECK(helper_ != NULL);
  return helper_->GetAzerEGLInterface();
}
}  // namespace skia
}  // namespace azer
