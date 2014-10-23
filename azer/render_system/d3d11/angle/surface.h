#pragma once

#include "base/basictypes.h"
#include "libEGL/Surface.h"

namespace azer {
class AzerSurface : public egl::Surface {
 public:
  AzerSurface();
  ~AzerSurface();

  /**
   * create swapchian
   */
  bool initialize();
  bool resetSwapChain();
 private:
  DISALLOW_COPY_AND_ASSIGN(AzerSurface);
};
}  // namespace azer
