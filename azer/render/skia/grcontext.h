#pragma once

#include "gl/SkGLContextHelper.h"
#include "gl/SkANGLEGLContext.h"

#include "base/basictypes.h"
#include "azer/render_system/d3d11/export.h"
#include "azer/render_system/d3d11/angle/angle.h"


struct GrGLInterface;

namespace azer {
namespace skia {
class SkAzerANGLEGrContext : public SkGLContextHelper {
 public:
  SkAzerANGLEGrContext(int width, int height);
  virtual ~SkAzerANGLEGrContext();

  virtual void makeCurrent() const OVERRIDE;
  virtual void swapBuffers() const OVERRIDE;

  angle::Context* GetAngleContext() { return &context_;}
 protected:
  virtual const GrGLInterface* createGLContext() OVERRIDE;
  virtual void destroyGLContext() OVERRIDE;
 private:
  // AutoContextRestore auto_restore_;
  angle::Context context_;
  int width_, height_;
  DISALLOW_COPY_AND_ASSIGN(SkAzerANGLEGrContext);
};
}  // namespace skia
}  // namespace azer
