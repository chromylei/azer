#include "azer/render_system/d3d11/angle/angle.h"

#include "GLES2/gl2.h"
// #include "GLES2/gl2ext.h"
// #include "libGLESv2/main.h"

namespace azer {
namespace angle {

bool AngleTexture::Init(int width, int height) {
  GLuint fb;
  GLuint tex = 0;
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
 
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               width, height,
               0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, 0,
                         GL_TEXTURE_2D, tex, 0);
  tex_ = tex;
  fb_ = fb;
  if (GL_NO_ERROR != glGetError()) {
    return false;
  }

  //HANDLE handle;
  useGL();
  //glGetFramebufferShareD3DTex(GL_DRAW_FRAMEBUFFER_ANGLE, fb, &handle);

  glBindTexture(GL_TEXTURE_2D, 0);
  return GL_NO_ERROR == glGetError();
}

AngleTexture::AngleTexture()
    : tex_(-1)
    , fb_(-1) {
}

AngleTexture::~AngleTexture() {
  if (tex_ != -1) {
    GLuint tex = (GLuint)tex_;
    glDeleteTextures(1, &tex);
  }
  if (fb_ != -1) {
    GLuint fb = (GLuint)fb_;
    glDeleteFramebuffers(1, &fb);
  }
}

void AngleTexture::useGL() {
  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)fb_);
}
void AngleTexture::resetGL() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace angle
}  // namespace azer
