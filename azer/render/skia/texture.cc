#include "azer/render/skia/texture.h"

#include "gl/GrGLFunctions.h"
#include "gl/GrGLDefines.h"
#include "gl/GrGLUtil.h"

#include "azer/render/skia/context.h"


namespace azer {
namespace skia {
AzerSkTexture::~AzerSkTexture() {
  const GrGLInterface* gl = context_->GetGrGLInterface();
  GR_GL_CALL(gl, DeleteFramebuffers(1, &fb_));
  GR_GL_CALL(gl, DeleteTextures(1, &texid_));
}

bool AzerSkTexture::Init() {
  const GrGLInterface* gl = context_->GetGrGLInterface();
  GR_GL_CALL(gl, GenFramebuffers(1, &fb_));

  GR_GL_CALL(gl, GenTextures(1, &texid_));
  GR_GL_CALL(gl, BindTexture(GR_GL_TEXTURE_2D, texid_));
  gl->fFunctions.fGetError();
  GR_GL_CALL(gl, TexImage2D(GR_GL_TEXTURE_2D, 0, GR_GL_RGBA,
                            width_, height_,
                            0, GR_GL_RGBA, GR_GL_UNSIGNED_BYTE, NULL));

  GR_GL_CALL(gl, FramebufferTexture2D(GR_GL_FRAMEBUFFER, GR_GL_COLOR_ATTACHMENT0,
                                      GR_GL_TEXTURE_2D, texid_, 0));
  GR_GL_CALL(gl, BindFramebuffer(GR_GL_FRAMEBUFFER, fb_));
  GR_GL_CALL(gl, Clear(GR_GL_COLOR_BUFFER_BIT));
  GR_GL_CALL(gl, ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
  gl->fFunctions.fGetError();
  return gl->fFunctions.fGetError() == GR_GL_NO_ERROR;
}
}  // namespace skia
}  // namespace azer
