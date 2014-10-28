#pragma once

#include <windows.h>
#include "azer/ui/window/window.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/export.h"

namespace azer {
namespace angle {
struct Context {
  void* display;
  void* context;
  void* surface;
  void* user_data;
  int width;
  int height;

  Context()
      : display(NULL)
      , context(NULL)
      , surface(NULL)
      , user_data(NULL)
      , width(0)
      , height(0) {
  }
};

AZER_D3D11RS_EXPORT bool InitAngle(window::NativeWindowHandle handle, bool pbuffer,
                                   Context* context);
AZER_D3D11RS_EXPORT void UninitializeAngle(Context* ctx);

AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(void* surface, Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetCurrentFramebufferTexture(Context* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetFramebufferTexture(void* framebuffer, Context* ctx);
}  // namespace angle
}  // namespace azer
