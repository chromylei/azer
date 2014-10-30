#pragma once

#include "azer/render/glcontext.h"

namespace azer {
class ANGLEGLInterface : public AzerEGLInterface {
 public:
  ANGLEGLInterface() {}
  ~ANGLEGLInterface() {}
  virtual bool Init(AzerEGLContext* context) OVERRIDE;
  virtual bool MakeCurrent(const AzerEGLContext* context) OVERRIDE;
  virtual void Destroy(AzerEGLContext* context) OVERRIDE;
  virtual AzerGLTexture* CreateTexture(int width, int height) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(ANGLEGLInterface);
};
}  // namespace azer
