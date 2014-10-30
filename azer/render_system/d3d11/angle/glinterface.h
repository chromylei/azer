#pragma once

#include "base/basictypes.h"
#include "azer/render/glcontext.h"
#include "azer/render/texture.h"

namespace azer {
class ANGLEGLInterface : public AzerEGLInterface {
 public:
  ANGLEGLInterface() {}
  ~ANGLEGLInterface() {}
  virtual bool Init(AzerEGLContext* context) OVERRIDE;
  virtual bool MakeCurrent(const AzerEGLContext* context) OVERRIDE;
  virtual void Destroy(AzerEGLContext* context) OVERRIDE;
  virtual Texture* CreateTexture(uint32 gltexid) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(ANGLEGLInterface);
};
}  // namespace azer
