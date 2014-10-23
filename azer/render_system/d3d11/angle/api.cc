#include "azer/render_system/d3d11/angle/api.h"

#include "azer/render_system/d3d11/angle/display.h"
#include "azer/render_system/d3d11/angle/precompile.h"
#include "azer/render_system/d3d11/angle/precompile.h"
#include "azer/ui/window/window_host.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"
#include "libEGL/Display.h"

namespace azer {
/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flag - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8
/// esCreateWindow flag - EGL_POST_SUB_BUFFER_NV supported.
#define ES_WINDOW_POST_SUB_BUFFER_SUPPORTED 16

bool InitializeDisplay(D3D11RenderSystem* rs, HWND hWnd, EGLDisplay* display,
                       EGLint* major, EGLint* minor) {
  AzerDisplay* azer_display = new AzerDisplay(GetDC(hWnd), GetDC(hWnd));
  if (!azer_display->initialize(rs)) {
    return false;
  }

  *display = (EGLDisplay)azer_display;
  return true;
}

bool InitAngle(RenderSystem* rrs, WindowHost* host) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)rrs;

  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

  GLuint flags = ES_WINDOW_RGB;
  EGLint configAttribList[] =
  {
    EGL_RED_SIZE,       5,
    EGL_GREEN_SIZE,     6,
    EGL_BLUE_SIZE,      5,
    EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
    EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
    EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
    EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
    EGL_NONE
  };

  /**
   * 与 egl 的实现的主要区别在于 display 的创建及初始化
   * 当然 renderer 也是其中一部分，renderer 的创建在 display 内部
   * 创建完成 display 及 renderer 之后的部分是一样的
   */
  HWND hWnd = (HWND)host->Handle();
  if (!InitializeDisplay(rs, hWnd, &display, &majorVersion, &minorVersion)) {
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

}  // namespace azer
