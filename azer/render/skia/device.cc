#include "azer/render/skia/device.h"

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

#include "azer/render/glcontext.h"

namespace azer {
namespace skia {


bool AzerSkDevice::Init2(Context* ctx, Canvas* canvas) {
  tex_.reset(ctx->GetAzerEGLInterface()->CreateTexture(
      canvas->width(), canvas->height()));
  if (!tex_.get()) {
    return false;
  }

  return true;
}

bool AzerSkDevice::Init(Context* ctx, Canvas* canvas) {
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

GrTexture* AzerSkDevice::GetCurrentColorTexture(Context* ctx, Canvas* canvas) {
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

bool AzerSkDevice::InitFromTexture(GrTexture* tex, GrContext* context) {
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

bool AzerSkDevice::InitForDefault(Context* ctx, Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  GrTexture* tex = GetCurrentColorTexture(ctx, canvas);
  if (tex == NULL) {
    return false;
  }

  return InitFromTexture(tex, context);
}

}  // namespace skia
}  // namespace azer
