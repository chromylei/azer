#pragma once

#include "azer/render/render_system.h"
#include "azer/render/skia/skia_context.h"

namespace azer {
namespace skia {

AZER_EXPORT Context* Init(int width, int height);
}  // namespace skia
}  // namespace azer
