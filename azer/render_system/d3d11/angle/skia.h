#pragma once

#include "azer/render_system/d3d11/export.h"
#include "azer/render_system/d3d11/angle/skia_context.h"


namespace azer {
namespace skia {

AZER_D3D11RS_EXPORT Context* Init(int width, int height);
}  // namespace skia
}  // namespace azer
