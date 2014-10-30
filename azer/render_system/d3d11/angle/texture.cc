#include "azer/render_system/d3d11/angle/texture.h"

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
// #include "libGLESv2/main.h"
#include "azer/render_system/d3d11/angle/angle.h"

namespace azer {
namespace angle {

AngleTexture::AngleTexture(int width, int height)
    : AzerGLTexture(width, height)
    , fb_(-1) {
}

AngleTexture::~AngleTexture() {
  if (tex_ != -1) {
    GLuint tex = (GLuint)glid();
    glDeleteTextures(1, &tex);
  }
  if (fb_ != -1) {
    GLuint fb = (GLuint)fb_;
    glDeleteFramebuffers(1, &fb);
  }
}
bool AngleTexture::Init() {
  GLuint fb;
  GLuint tex = 0;
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
 
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               width(), height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, 0,
                         GL_TEXTURE_2D, tex, 0);
  tex_ = tex;
  fb_ = fb;
  glid_ = tex;
  if (GL_NO_ERROR != glGetError()) {
    return false;
  }

  //HANDLE handle;
  //glGetFramebufferShareD3DTex(GL_DRAW_FRAMEBUFFER_ANGLE, fb, &handle);

  glBindTexture(GL_TEXTURE_2D, 0);
  return GL_NO_ERROR == glGetError();
}

}  // namespace angle
}  // namespace azer
