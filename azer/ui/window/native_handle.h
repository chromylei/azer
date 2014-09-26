#pragma once

#include "base/basictypes.h"

namespace azer {
namespace window {
#if defined(OS_WIN)
typedef uint32 NativeWindowHandle;
const NativeWindowHandle kInvalidWindowHandle = 0;
#endif
}  // namespace window
}  // namespace azer
