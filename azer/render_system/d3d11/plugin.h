#pragma once

#include "azer/render_system/d3d11/export.h"

namespace azer {
class RenderSystem;
class WindowHost;
}  // namespace azer

extern "C" AZER_D3D11RS_EXPORT azer::RenderSystem* CreateRenderSystem(azer::WindowHost* win);
