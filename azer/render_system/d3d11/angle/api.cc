#include "azer/render_system/d3d11/angle/api.h"

#include "azer/render_system/d3d11/angle/precompile.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"
#include "libEGL/Display.h"

bool InitializeDisplay(EGLDisplay* display, EGLint* major, EGLint* minor) {
  AzerDisplay* azer_display = new AzerDisplay(GetDC(hWnd), GetDC(hWnd));
  if (!azer_displar->initialize(rs)) {
    return false;
  }

  *display = (EGLDisplay)azer_display;
  return true;
}

bool InitAngle(RenderSystem* rrs) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)rrs;

  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
  
  if (!InitializeDisplay(&display, &majorVersion, &minorVersion)) {
    return false;
  }

  // Get configs
  if (!eglGetConfigs(display, NULL, 0, &numConfigs)) {
    return false;
  }

  if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs)) {
    return false;
  }

  context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
  if (context == EGL_NO_CONTEXT) {
    return EGL_FALSE;
  }


  return true;
}
