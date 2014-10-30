#pragma once

#include "base/basictypes.h"
#include "azer/render/skia/context.h"
#include "azer/render/skia/grcontext.h"

namespace azer {
namespace skia {
class Context;
class AzerSkTexture {
 public:
  AzerSkTexture(int width, int height, Context* ctx)
      : width_(width)
      , height_(height)
      , fb_(-1)
      , texid_(-1)
      , context_(ctx) {
  }

  ~AzerSkTexture();

  bool Init();
  uint32 texid() const { return texid_;}
 private:
  int width_;
  int height_;
  uint32 fb_;
  uint32 texid_;
  Context* context_;
  DISALLOW_COPY_AND_ASSIGN(AzerSkTexture);
};
}  // namespace skia
}  // namespace azer
