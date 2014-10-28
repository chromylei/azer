#pragma once

#include <windows.h>
#include "azer/ui/window/window.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/export.h"

namespace azer {
namespace angle {
AZER_D3D11RS_EXPORT bool InitAngle(window::NativeWindowHandle handle);
AZER_D3D11RS_EXPORT void UninitializeAngle();

AZER_D3D11RS_EXPORT TexturePtr GetSurface(int64 surface);
AZER_D3D11RS_EXPORT TexturePtr GetSurface();
}  // namespace angle
}  // namespace azer
