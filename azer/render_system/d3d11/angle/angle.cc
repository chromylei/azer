
#include "azer/render_system/d3d11/angle/angle.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/ui/window/window_host.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"

PFNEGLQUERYSURFACEPOINTERANGLEPROC eglQuerySurfacePointerANGLE;

namespace azer {
namespace angle {
EGLint configAttribList[] = {
  EGL_RED_SIZE,       5,
  EGL_GREEN_SIZE,     6,
  EGL_BLUE_SIZE,      5,
  EGL_ALPHA_SIZE,     8,
  EGL_DEPTH_SIZE,     24,
  EGL_STENCIL_SIZE,   8,
  EGL_SAMPLE_BUFFERS, 0,
  EGL_NONE
};

D3D11Texture2DShared* CreatePBufferTexture(RenderSystem* rrs, Context* ctx) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)rrs;
  Texture::Options options;
  options.width = ctx->width;
  options.height = ctx->height;
  options.format = azer::kRGBAn8;
  options.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  options.sampler.sample_level = 1;
  options.usage = GraphicBuffer::kDefault;

  std::unique_ptr<D3D11Texture2DShared> tex(new D3D11Texture2DShared(options, rs));
  if (!tex->Init(NULL, 1)) {
    return NULL;
  }

  // ID3D11Resource* resource = tex->CreateSharedResource();
  return tex.release();
}

bool Init(RenderSystem* rs, Context* ctx) {
  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

  WindowHost* host = rs->GetRenderWindowHost();
  EGLNativeWindowType hWnd = (EGLNativeWindowType)host->Handle();

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

  D3D11Texture2DShared* shared = CreatePBufferTexture(rs, ctx);
  ctx->tex.reset(shared);
  EGLint surfaceAttribList[] =  {
    EGL_WIDTH,   -1,
    EGL_HEIGHT,  -1,
    EGL_NONE, EGL_NONE
  };
  surfaceAttribList[1] = ctx->width;
  surfaceAttribList[3] = ctx->height;
  EGLClientBuffer egl_buffer = (EGLClientBuffer)(shared->GetSharedHanle());
  surface = eglCreatePbufferFromClientBuffer(display,
                                             EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
                                             egl_buffer,
                                             config,
                                             surfaceAttribList);
  if (surface == EGL_NO_SURFACE) {
    LOG(ERROR) << "Failed to create Surface";
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

  ctx->display = display;
  ctx->surface = surface;
  ctx->context = context;

  eglQuerySurfacePointerANGLE =
      (PFNEGLQUERYSURFACEPOINTERANGLEPROC)eglGetProcAddress("eglQuerySurfacePointerANGLE");
  if (eglQuerySurfacePointerANGLE == NULL) {
    return false;
  }

  return true;
}

void UninitializeAngle(Context* ctx) {
}

TexturePtr GetSurfaceTexture(Context* ctx) {
  return GetSurfaceTexture(ctx->surface, ctx);
}

TexturePtr GetSurfaceTexture(void* sur, Context* context) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)RenderSystem::Current();
  ID3D11Texture2D* resource = NULL;
  EGLint config = EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE;
  EGLSurface surface = (EGLSurface)sur;
  if (EGL_FALSE == eglQuerySurfacePointerANGLE(context->display,
                                               surface,
                                               config,
                                               (void**)&resource)) {
    SAFE_RELEASE(resource);
    return TexturePtr();
  }
  
  return TexturePtr(D3D11Texture2DShared::Create(resource, rs));
}

TexturePtr GetFramebufferTexture(void* framebuffer, Context* ctx) {
  return TexturePtr();
}

TexturePtr GetCurrentFramebufferTexture(Context* ctx) {
  EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
  if (surface == EGL_NO_SURFACE) {
    return TexturePtr();
  }

  return GetSurfaceTexture((void*)surface, ctx);
}
}  // namespace angle
}  // namespace azer
