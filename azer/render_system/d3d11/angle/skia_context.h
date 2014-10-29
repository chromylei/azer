#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render_system/d3d11/export.h"
#include "azer/render_system/d3d11/texture.h"

class GrContext;
struct GrGLInterface;
class SkGpuDevice;
class SkCanvas;

namespace azer {

namespace angle {
struct Context;
}  // namespace
  
namespace skia {

class Context;
class Device;
class SkAzerANGLEGrContext;

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
  angle::Context* GetAngleContext();

  GrContext* GetGrContext() { return gr_context_;}
  const GrGLInterface* GetGrGLInterface() { return interface_;}
 private:
  GrContext* gr_context_;
  const GrGLInterface* interface_;
  SkAzerANGLEGrContext* helper_;
  int32 width_;
  int32 height_;
  friend class Device;
  DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace skia
}  // namespace azer
