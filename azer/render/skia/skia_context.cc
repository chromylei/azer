#include "azer/render_system/d3d11/angle/skia_context.h"

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

#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/angle/grcontext.h"
#include "base/strings/string_util.h"

namespace azer {
namespace skia {

class Device {
 public:
  Device(){}
  ~Device() {}

  bool Init(Context* ctx, Canvas* canvas);
  bool InitForDefault(Context* ctx, Canvas* canvas);
  bool Init2(Context* ctx, Canvas* canvas);
  bool InitFromTexture(GrTexture* tex, GrContext* context);

  SkCanvas* GetCanvas() { return sk_canvas_.get();}
  SkGpuDevice* GetDevice() { return gr_device_.get();}
 private:
  GrTexture* GetCurrentColorTexture(Context* context, Canvas* canvas);
  std::unique_ptr<SkGpuDevice> gr_device_;
  std::unique_ptr<SkCanvas> sk_canvas_;
  angle::AngleTexture angle_tex_;
  DISALLOW_COPY_AND_ASSIGN(Device);
};

bool Device::Init2(Context* ctx, Canvas* canvas) {
  if (!angle_tex_.Init(canvas->width(), canvas->height())) {
    return false;
  }

  return true;
}

bool Device::Init(Context* ctx, Canvas* canvas) {
  // skia/tests/SkGpuDevice.cpp
  GrContext* context = ctx->gr_context_;
  SkImageInfo info = SkImageInfo::Make(canvas->width(), canvas->height(),
                                       kBGRA_8888_SkColorType, kOpaque_SkAlphaType);
  gr_device_.reset(SkGpuDevice::Create(context, info, 0));
  if (gr_device_.get() == NULL) {
    LOG(ERROR) << "Failed to create SkGpuDevice";
    return false;
  }

  sk_canvas_.reset(new SkCanvas(gr_device_.get()));
  if (sk_canvas_.get() == NULL) {
    LOG(ERROR) << "Failed to create SkCanvas";
    return false;
  }
  return true;
}

GrTexture* Device::GetCurrentColorTexture(Context* ctx, Canvas* canvas) {
  // skia/tests/SkGpuDevice.cpp
  GrContext* context = ctx->gr_context_;
  const GrGLInterface* intf = ctx->GetGrGLInterface();
  GrBackendTextureDesc desc;
  GrGLint buffer;
  GR_GL_GetIntegerv(intf, GR_GL_COLOR_ATTACHMENT0, &buffer);
  desc.fFlags = kRenderTarget_GrBackendTextureFlag;
  desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
  desc.fWidth = ctx->width();
  desc.fHeight = ctx->height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fSampleCnt = 0;
  desc.fTextureHandle = buffer;

  return context->wrapBackendTexture(desc);
}

bool Device::InitFromTexture(GrTexture* tex, GrContext* context) {
  DCHECK(tex != NULL);
  DCHECK(context != NULL);
  gr_device_.reset(new SkGpuDevice(context, tex));
  if (gr_device_.get() == NULL) {
    LOG(ERROR) << "Failed to create SkGpuDevice";
    return false;
  }

  sk_canvas_.reset(new SkCanvas(gr_device_.get()));
  if (sk_canvas_.get() == NULL) {
    LOG(ERROR) << "Failed to create SkCanvas";
    return false;
  }
  return true;
}

bool Device::InitForDefault(Context* ctx, Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  GrTexture* tex = GetCurrentColorTexture(ctx, canvas);
  if (tex == NULL) {
    return false;
  }

  return InitFromTexture(tex, context);
}

// class Canvas
Canvas::Canvas(int width, int height)
    : width_(width)
    , height_(height)
    , device_(NULL) {
}

Canvas::~Canvas() {
  if (device_) { delete device_;}
}

bool Canvas::Init(Context* ctx, bool is_default) {
  device_ = new Device();
  if (is_default) {
    return device_->Init2(ctx, this);
  } else {
    return device_->Init(ctx, this);
  }
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
Context::Context(int width, int height)
    : gr_context_(NULL)
    , interface_(NULL)
    , helper_(NULL)
    , width_(width)
    , height_(height) {
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
  helper_ = new SkAzerANGLEGrContext(width(), height());
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
  if (ptr->Init(this, false)) {
    return CanvasPtr(ptr.release());
  } else {
    return CanvasPtr();
  }
}

CanvasPtr Context::GetDefault() {
  angle::GetSurfaceTexture(GetAngleContext()->surface, GetAngleContext());
  std::unique_ptr<Canvas> ptr(new Canvas(width_, height_));
  if (ptr->Init(this, true)) {
    return CanvasPtr(ptr.release());
  } else {
    return CanvasPtr();
  }
}

void Context::resize(int width, int height) {
  CHECK(false);
}

angle::Context* Context::GetAngleContext() {
  DCHECK(helper_ != NULL);
  return helper_->GetAngleContext();
}
}  // namespace skia
}  // namespace azer
