#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render_system/d3d11/export.h"
#include "azer/render_system/d3d11/texture.h"

class GrContext;
class SkGpuDevice;
class SkCanvas;
class SkGLContextHelper;

namespace azer {
namespace skia {

class Context;
class Device;
class AZER_D3D11RS_EXPORT Canvas {
 public:
  Canvas(int width, int height);
  ~Canvas();

  TexturePtr& GetTexture() { return texture_;}
  bool Init(Context* context, bool is_default = false);

  int width() const { return width_;}
  int height() const { return height_;}

  bool Save(const ::base::FilePath& path);
  SkCanvas* GetCanvas();
 private:
  TexturePtr texture_;
  int32 width_;
  int32 height_;
  Device* device_;
  friend class Context;
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};

typedef std::shared_ptr<Canvas> CanvasPtr;

class AZER_D3D11RS_EXPORT Context {
 public:
  Context(int width, int height);
  ~Context();

  bool Init();
  void resize(int width, int height);

  int width() const { return width_;}
  int height() const { return height_;}

  CanvasPtr CreateCanvas(int width, int height);
  CanvasPtr GetDefault();
 private:
  GrContext* gr_context_;
  SkGLContextHelper* helper_;
  int32 width_;
  int32 height_;
  friend class Device;
  DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace skia
}  // namespace azer
