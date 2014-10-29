#pragma once

#include <windows.h>
#include "azer/ui/window/window.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/export.h"
#include "azer/render/render_target.h"
#include "azer/render/renderer.h"

namespace azer {
namespace angle {
struct Context {
  void* display;
  void* context;
  void* surface;
  void* user_data;
  int width;
  int height;
  TexturePtr tex;

  Context()
      : display(NULL)
      , context(NULL)
      , surface(NULL)
      , user_data(NULL)
      , width(0)
      , height(0) {
  }
};

AZER_D3D11RS_EXPORT bool Init(RenderSystem* rs, Context* context);
AZER_D3D11RS_EXPORT void UninitializeAngle(Context* ctx);

AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(void* surface, Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetCurrentFramebufferTexture(Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetFramebufferTexture(void* framebuffer, Context* ctx);
}  // namespace angle
}  // namespace azer
