#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/render/render_system.h"

class SkCanvas;

namespace azer {
namespace skia {

class AzerSkDevice;
class Context;

class AZER_EXPORT Canvas {
 public:
  Canvas(int width, int height, Context* context);
  ~Canvas();

  TexturePtr& GetTexture();
  bool Init();

  int width() const { return width_;}
  int height() const { return height_;}

  void flush();

  bool Save(const ::base::FilePath& path);
  SkCanvas* GetSkCanvas();
 private:
  TexturePtr texture_;
  int32 width_;
  int32 height_;
  AzerSkDevice* device_;
  Context* context_;
  friend class Context;
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};

typedef std::shared_ptr<Canvas> CanvasPtr;

}  // namespace skia
}  // namespace azer
