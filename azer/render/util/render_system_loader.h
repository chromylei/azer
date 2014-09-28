#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"


namespace azer {

class WindowHost;

bool AZER_EXPORT LoadRenderSystem(WindowHost* host);
}  // namespace azer
