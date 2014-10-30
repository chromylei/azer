#include "azer/render/skia/context.h"

#include "SkCanvas.h"
#include "SkGpuDevice.h"
#include "GrContext.h"
#include "GrTexture.h"
#include "GrRenderTarget.h"
#include "SkImageInfo.h"
#include "SkImageEncoder.h"
#include "gl/GrGLInterface.h"
#include "gl/SkGLContextHelper.h"
#include "gl/GrGLFunctions.h"
#include "gl/GrGLDefines.h"
#include "gl/GrGLUtil.h"

#include "azer/render/texture.h"
#include "azer/render/skia/grcontext.h"
#include "azer/render/skia/device.h"
#include "base/strings/string_util.h"

namespace azer {
namespace skia {

// class Canvas
Canvas::Canvas(int width, int height)
    : width_(width)
    , height_(height)
    , device_(NULL) {
}

Canvas::~Canvas() {
  if (device_) { delete device_;}
}

bool Canvas::Init(Context* ctx) {
  device_ = new AzerSkDevice();
  return device_->Init(ctx, this);
}

SkCanvas* Canvas::GetCanvas() {
  DCHECK(device_ != NULL);
  return device_->GetCanvas();
}

namespace {
SkImageEncoder::Type ImageType(const ::base::FilePath::StringType& ext) {
  if (ext ==  FILE_PATH_LITERAL(".bmp")) {
    return SkImageEncoder::kBMP_Type;
  } else if (ext == FILE_PATH_LITERAL(".png")) {
    return SkImageEncoder::kPNG_Type;
  } else if (ext == FILE_PATH_LITERAL(".jpg") || ext == FILE_PATH_LITERAL(".jpeg")) {
    return SkImageEncoder::kJPEG_Type;
  } else if (ext == FILE_PATH_LITERAL(".gif")) {
    return SkImageEncoder::kGIF_Type;
  } else {
    NOTREACHED();
    return SkImageEncoder::kBMP_Type;
  }
}
}

bool Canvas::Save(const ::base::FilePath& path) {
  DCHECK(device_ != NULL);
  const ::base::FilePath::StringType ext = StringToLowerASCII(path.Extension());
  SkImageEncoder::Type type = ImageType(ext);
  std::string pathstr = ::base::WideToUTF8(path.value());
  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, width(), height());
  bitmap.allocPixels();
  device_->GetCanvas()->readPixels(&bitmap, 0, 0);
  return SkImageEncoder::EncodeFile(pathstr.c_str(), bitmap, type, 100);
}

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
  helper_ = new AzerSkiaGrContext(1, 1);
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
  std::unique_ptr<Canvas> ptr(new Canvas(width, height));
  if (ptr->Init(this)) {
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
