
#include "azer/render_system/d3d11/angle/angle.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"

namespace azer {
EGLint configAttribList[] = {
  EGL_RED_SIZE,       8,
  EGL_GREEN_SIZE,     8,
  EGL_BLUE_SIZE,      8,
  EGL_ALPHA_SIZE,     8,
  EGL_DEPTH_SIZE,     24,
  EGL_STENCIL_SIZE,   8,
  EGL_SAMPLE_BUFFERS, 1,
  EGL_NONE
};

EGLint surfaceAttribList[] =  {
  EGL_POST_SUB_BUFFER_SUPPORTED_NV, EGL_TRUE,
  EGL_NONE, EGL_NONE
};

namespace {
EGLDisplay eglDisplay = 0;
EGLSurface eglSurface = 0;
EGLContext eglContext = 0;
}  // namespace


bool InitAngle(window::NativeWindowHandle handle) {
  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

  EGLNativeWindowType hWnd = (EGLNativeWindowType)handle;

  display = eglGetDisplay(GetDC(hWnd));
  if (display == EGL_NO_DISPLAY) {
    return false;
  }

  // Initialize EGL
  if (!eglInitialize(display, &majorVersion, &minorVersion)) {
    return false;
  }

  // Get configs
  if (!eglGetConfigs(display, NULL, 0, &numConfigs)) {
    return false;
  }

  // Choose config
  if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs)) {
    return false;
  }

  surface = eglCreatePbufferSurface(display, config, surfaceAttribList);
  if (surface == EGL_NO_SURFACE) {
    return false;
  }

  // Create a GL context
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
  if (context == EGL_NO_CONTEXT) {
    return EGL_FALSE;
  }

  // Make the context current
  if (!eglMakeCurrent(display, surface, surface, context)) {
    return EGL_FALSE;
  }

  eglDisplay = display;
  eglSurface = surface;
  eglContext = context;

  return true;
}
}  // namespace azer
