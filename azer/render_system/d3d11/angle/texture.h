#include "azer/render/glcontext.h"
#include "azer/render_system/d3d11/texture.h"

namespace azer {
namespace angle {

class AngleTexture : public AzerGLTexture {
 public:
  AngleTexture(int32 width, int height);
  ~AngleTexture();
  bool Init();
 private:
  int32 fb_;
  int32 tex_;
  DISALLOW_COPY_AND_ASSIGN(AngleTexture);
};


}  // namespace angle
}  // namespace azer
