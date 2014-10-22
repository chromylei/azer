#include "azer/render_system/d3d11/angle/display.h"
#include "azer/render_system/d3d11/angle/renderer.h"

#include "libEGL/main.h"
#include "libEGL/Config.h"

namespace azer {

using egl::ConfigSet;
using egl::Config;

AzerDisplay::AzerDisplay(EGLNativeDisplayType displayId, HDC deviceContext)
    : egl::Display(displayId, deviceContext) {
  Display::setDisplay(displayId, this);
}

bool AzerDisplay::initialize(D3D11RenderSystem* rs) {
  if (isInitialized()) {
    return true;
  }

  AzerRenderer* renderer = new AzerRenderer(this, mDc, rs);
  if (EGL_SUCCESS != renderer->initialize()) {
    terminate();
    return false;
  }

  mRenderer = renderer;
  if (!mRenderer) {
    terminate();
    return false;
  }

  EGLint minSwapInterval = mRenderer->getMinSwapInterval();
  EGLint maxSwapInterval = mRenderer->getMaxSwapInterval();
  EGLint maxTextureWidth = mRenderer->getMaxTextureWidth();
  EGLint maxTextureHeight = mRenderer->getMaxTextureHeight();

  rx::ConfigDesc *descList;
  int numConfigs = mRenderer->generateConfigs(&descList);
  ConfigSet configSet;

  for (int i = 0; i < numConfigs; ++i)
    configSet.add(descList[i], minSwapInterval, maxSwapInterval,
                  maxTextureWidth, maxTextureHeight);

  // Give the sorted configs a unique ID and store them internally
  EGLint index = 1;
  for (ConfigSet::Iterator config = configSet.mSet.begin();
       config != configSet.mSet.end(); config++) {
    Config configuration = *config;
    configuration.mConfigID = index;
    index++;

    mConfigSet.mSet.insert(configuration);
  }

  mRenderer->deleteConfigs(descList);
  descList = NULL;

  if (!isInitialized()) {
    terminate();
    return false;
  }

  initExtensionString();
  initVendorString();

  return true;
}

}  // namespace azer
