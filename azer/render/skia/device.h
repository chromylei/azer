#include "azer/render/skia/context.h"

#include "azer/render/glcontext.h"
#include "azer/render/skia/texture.h"

struct GrGLInterface;
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
  bool InitForDefault(Context* ctx, Canvas* canvas);
  bool Init2(Context* ctx, Canvas* canvas);
  bool InitFromTexture(GrTexture* tex, GrContext* context);

  SkCanvas* GetCanvas() { return sk_canvas_.get();}
  SkGpuDevice* GetDevice() { return gr_device_.get();}
 private:
  GrTexture* GetTexture(GrGLuint texid, Context* context, Canvas* canvas);
  GrTexture* GetCurrentColorTexture(Context* context, Canvas* canvas);
  std::unique_ptr<SkGpuDevice> gr_device_;
  std::unique_ptr<SkCanvas> sk_canvas_;
  TexturePtr tex_;
  std::unique_ptr<AzerSkTexture> gltex_;
  DISALLOW_COPY_AND_ASSIGN(AzerSkDevice);
};

}  // namespace skia
}  // namespace azer
