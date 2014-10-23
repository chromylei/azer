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

/*
EGLSurface AzerDisplay::createWindowSurface(HWND hWnd, EGLConfig config,
                                            const EGLint* attribList) {
  const Config *configuration = mConfigSet.get(config);
  EGLint postSubBufferSupported = EGL_FALSE;
  if (attribList) {
    while (*attribList != EGL_NONE) {
      switch (attribList[0]) {
        case EGL_RENDER_BUFFER:
          switch (attribList[1]){
            case EGL_BACK_BUFFER:
              break;
            case EGL_SINGLE_BUFFER:
              // Rendering directly to front buffer not supported
              return (EGL_BAD_MATCH, EGL_NO_SURFACE);
            default:
              return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
          }
          break;
        case EGL_POST_SUB_BUFFER_SUPPORTED_NV:
          postSubBufferSupported = attribList[1];
          break;
        case EGL_VG_COLORSPACE:
          return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
        case EGL_VG_ALPHA_FORMAT:
          return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
        default:
          return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
      }
      attribList += 2;
    }
  }

  Surface* surface = new Surface(this, configuration, window,
                                 postSubBufferSupported);
  if (surface->initialize()) {
    delete surface;
    return EGL_NO_SURFACE;
  }
  mSurfaceSet.insert(surface);
  return success(surface);
}
*/
}  // namespace azer
