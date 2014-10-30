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
#include "azer/render/texture.h"

namespace azer {
namespace skia {

bool AzerSkDevice::Init(Context* ctx, Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  grtex_ = CreateTex(ctx, canvas);
  // tex_.reset(ctx->GetAzerEGLInterface()->CreateTexture(gltex_->fbid()));
  gr_device_.reset(SkGpuDevice::Create(grtex_));
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

GrTexture* AzerSkDevice::CreateTex(Context* ctx, Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  GrTextureDesc desc;
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fFlags = kRenderTarget_GrTextureFlagBit;
  desc.fWidth = canvas->height();
  desc.fHeight = canvas->height();
  desc.fSampleCnt = 0;
  return context->createUncachedTexture(desc, NULL, 0);
}

/*
GrTexture* AzerSkDevice::GetTexture(GrGLuint texid, Context* ctx, Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  GrBackendTextureDesc desc;
  desc.fFlags = kRenderTarget_GrBackendTextureFlag;
  desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
  desc.fWidth = canvas->width();
  desc.fHeight = canvas->height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fSampleCnt = 0;
  desc.fTextureHandle = texid;
  return context->wrapBackendTexture(desc);
}

GrRenderTarget* AzerSkDevice::GetRenderTarget(GrGLuint fbid, Context* ctx,
                                              Canvas* canvas) {
  GrContext* context = ctx->gr_context_;
  GrBackendRenderTargetDesc desc;
  desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
  desc.fWidth = canvas->width();
  desc.fHeight = canvas->height();
  desc.fConfig = kSkia8888_GrPixelConfig;
  desc.fSampleCnt = 0;
  desc.fRenderTargetHandle = 0;
  GrRenderTarget* target = context->wrapBackendRenderTarget(desc);
  context->setRenderTarget(target);
  return target;
}
*/

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

}  // namespace skia
}  // namespace azer
