#pragma once

#include <windows.h>
#include "azer/ui/window/window.h"

namespace azer {
bool InitAngle(window::NativeWindowHandle handle);
void UninitializeAngle();
}  // namespace azer
