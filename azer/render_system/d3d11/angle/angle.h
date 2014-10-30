#pragma once

#include <windows.h>
#include "azer/ui/window/window.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/export.h"
#include "azer/render/render_target.h"
#include "azer/render/renderer.h"
#include "azer/render/glcontext.h"
#include "azer/render/render_system.h"

namespace azer {
namespace angle {

class AngleTexture {
 public:
  AngleTexture();
  ~AngleTexture();
  bool Init(int width, int height);
  TexturePtr GetTexture() { return texture_;}

  int64 texid() { return tex_;}
  int64 fbid() { return fb_;}

  // helper function set openGL Context
  void useGL();
  void resetGL();
 private:
  int64 tex_;
  int64 fb_;
  TexturePtr texture_;
  DISALLOW_COPY_AND_ASSIGN(AngleTexture);
};

AZER_D3D11RS_EXPORT bool Init(RenderSystem* rs, AzerGLContext* context);
AZER_D3D11RS_EXPORT void Destroy(AzerGLContext* ctx);

AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(AzerGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(void* surface, AzerGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetCurrentFramebufferTexture(AzerGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetFramebufferTexture(void* framebuffer,
                                                     AzerGLContext* ctx);
}  // namespace angle
}  // namespace azer
