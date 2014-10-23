#include "azer/render_system/d3d11/angle/surface.h"

#include "azer/render_system/d3d11/angle/display.h"
#include "azer/render_system/d3d11/angle/renderer.h"
#include "base/logging.h"

namespace azer {
bool AzerSurface::initialize() {
  if (!resetSwapChain()) {
    return false;
  }
  return true;
}

bool AzerSurface::resetSwapChain() {
  CHECK(!mSwapChain);
  int width;
  int height;
  if (mWindow) {
    RECT windowRect;
    if (!GetClientRect(getWindowHandle(), &windowRect)) {
      ASSERT(false);

      ERR("Could not retrieve the window dimensions");
      return error(EGL_BAD_SURFACE, false);
    }

    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
  } else {
    // non-window surface - size is determined at creation
    width = mWidth;
    height = mHeight;
  }

  AzerRenderer* renderer = (AzerRenderer*)mRenderer;
  mSwapChain = renderer->createSwapChain(mWindow, mShareHandle,
                                         mConfig->mRenderTargetFormat,
                                         mConfig->mDepthStencilFormat);
  if (!mSwapChain)
  {
    return error(EGL_BAD_ALLOC, false);
  }

  if (!resetSwapChain(width, height))
  {
    delete mSwapChain;
    mSwapChain = NULL;
    return false;
  }

  return true;
}

}  // namespace azer
