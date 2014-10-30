#include "azer/render/skia/context.h"

#include "azer/render/glcontext.h"

struct GrGLInterface;
class GrRenderTarget;
class GrContext;
class GrTexture;
class SkGpuDevice;
class SkCanvas;

namespace azer {
namespace skia {

class AzerSkDevice {
 public:
  AzerSkDevice(){}
  ~AzerSkDevice() {}

  bool Init(Context* ctx, Canvas* canvas);
  bool InitFromTexture(GrTexture* tex, GrContext* context);

  SkCanvas* GetCanvas() { return sk_canvas_.get();}
  SkGpuDevice* GetDevice() { return gr_device_.get();}
  GrTexture* GetGrTex() { return grtex_;}
 private:
  GrTexture* CreateTex(Context* context, Canvas* canvas);
  // GrRenderTarget* GetRenderTarget(GrGLuint texid, Context* context, Canvas* canvas);
  // GrTexture* GetTexture(GrGLuint texid, Context* context, Canvas* canvas);
  std::unique_ptr<SkGpuDevice> gr_device_;
  std::unique_ptr<SkCanvas> sk_canvas_;
  GrTexture* grtex_;
  DISALLOW_COPY_AND_ASSIGN(AzerSkDevice);
};

}  // namespace skia
}  // namespace azer
