#include "azer/render/skia/canvas.h"


#include "gl/GrGLFunctions.h"
#include "gl/GrGLDefines.h"
#include "gl/GrGLUtil.h"
#include "SkCanvas.h"
#include "SkGpuDevice.h"
#include "GrTexture.h"
#include "GrContext.h"
#include "SkImageInfo.h"
#include "SkImageEncoder.h"

#include "azer/render/skia/device.h"
#include "azer/render/skia/context.h"
#include "base/strings/string_util.h"
#include "azer/render/texture.h"

namespace azer {
namespace skia {

// class Canvas
Canvas::Canvas(int width, int height, Context* ctx)
    : width_(width)
    , height_(height)
    , device_(NULL)
    , context_(ctx) {
}

Canvas::~Canvas() {
  if (device_) { delete device_;}
}

bool Canvas::Init() {
  device_ = new AzerSkDevice();
  return device_->Init(context_, this);
}

SkCanvas* Canvas::GetSkCanvas() {
  DCHECK(device_ != NULL);
  return device_->GetCanvas();
}

TexturePtr& Canvas::GetTexture() {
  if (!texture_.get()) {
    GrTexture* tex = device_->GetGrTex();
    texture_.reset(context_->GetAzerEGLInterface()->
               GetShareTextureFromTex(tex->getTextureHandle()));
  }
  return texture_;
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

}  // namespace skia
}  // namespace azer
