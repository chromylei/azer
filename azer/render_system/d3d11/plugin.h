#pragma once

#include "azer/render_system/common/export.h"

namespace azer {
class RenderSystem;
class WindowHost;
}  // namespace azer

extern "C" AZER_RS_EXPORT azer::RenderSystem* CreateRenderSystem(azer::WindowHost* win);
