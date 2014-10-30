#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render/render_system.h"
#include "azer/render/glcontext.h"

struct GrGLInterface;
class GrContext;
class SkGpuDevice;
class SkCanvas;

namespace azer {
namespace skia {

class AzerSkiaGrContext;
class Context;
class AzerSkDevice;

class AZER_EXPORT Canvas {
 public:
  Canvas(int width, int height);
  ~Canvas();

  TexturePtr& GetTexture() { return texture_;}
  bool Init(Context* context);

  int width() const { return width_;}
  int height() const { return height_;}

  bool Save(const ::base::FilePath& path);
  SkCanvas* GetCanvas();
 private:
  TexturePtr texture_;
  int32 width_;
  int32 height_;
  AzerSkDevice* device_;
  friend class Context;
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};

typedef std::shared_ptr<Canvas> CanvasPtr;

class AZER_EXPORT Context {
 public:
  Context();
  ~Context();

  bool Init();

  CanvasPtr CreateCanvas(int width, int height);

  AzerEGLContext* GetAzerEGLContext();
  AzerEGLInterface* GetAzerEGLInterface();

  GrContext* GetGrContext() { return gr_context_;}
  const GrGLInterface* GetGrGLInterface() { return interface_;}
 private:
  GrContext* gr_context_;
  const GrGLInterface* interface_;
  AzerSkiaGrContext* helper_;
  friend class AzerSkDevice;
  DISALLOW_COPY_AND_ASSIGN(Context);
};

}  // namespace skia
}  // namespace azer
