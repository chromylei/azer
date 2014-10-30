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

AZER_D3D11RS_EXPORT bool Init(RenderSystem* rs, AzerEGLContext* context);
AZER_D3D11RS_EXPORT void Destroy(AzerEGLContext* ctx);

AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(AzerEGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetSurfaceTexture(void* surface, AzerEGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetCurrentFramebufferTexture(AzerEGLContext* ctx);
AZER_D3D11RS_EXPORT TexturePtr GetFramebufferTexture(void* framebuffer,
                                                     AzerEGLContext* ctx);
}  // namespace angle
}  // namespace azer
