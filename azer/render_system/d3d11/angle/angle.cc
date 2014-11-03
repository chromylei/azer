
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
namespace {
const int kMaxEGLAttrNum = 256;
void InitDeviceConfigAttr(AzerEGLContext* context, EGLint* buf, int size);
}  // namespace

D3D11Texture2DShared* CreatePBufferTexture(RenderSystem* rrs, AzerEGLContext* ctx) {
  D3D11RenderSystem* rs = (D3D11RenderSystem*)rrs;
  Texture::Options options;
  options.width = ctx->width;
  options.height = ctx->height;
  options.format = ctx->colorbuf_format;
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

bool Init(RenderSystem* rs, AzerEGLContext* ctx) {
  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

  WindowHost* host = rs->GetWindowHost();
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
  EGLint device_config_list[kMaxEGLAttrNum];
  InitDeviceConfigAttr(ctx, device_config_list, arraysize(device_config_list));
  if (!eglChooseConfig(display, device_config_list, &config, 1, &numConfigs)) {
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
  /*
  EGLClientBuffer egl_buffer = (EGLClientBuffer)(shared->GetSharedHanle());
  surface = eglCreatePbufferFromClientBuffer(display,
                                             EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
                                             egl_buffer,
                                             config,
                                             surfaceAttribList);
  */
  surface = eglCreatePbufferSurface(display, config, surfaceAttribList);
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

void Destroy(AzerEGLContext* ctx) {
  if (ctx->display) {
    eglMakeCurrent((EGLDisplay)ctx->display, 0, 0, 0);
    if (ctx->context) {
      eglDestroyContext((EGLDisplay)ctx->display, (EGLContext)ctx->context);
    }

    if (ctx->surface) {
      eglDestroySurface((EGLDisplay)ctx->display, (EGLSurface)ctx->surface);
    }
  }
}

TexturePtr GetSurfaceTexture(AzerEGLContext* ctx) {
  return GetSurfaceTexture(ctx->surface, ctx);
}

TexturePtr GetSurfaceTexture(void* sur, AzerEGLContext* context) {
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
  
  return TexturePtr(D3D11Texture2DExtern::Create(resource, rs));
}

TexturePtr GetFramebufferTexture(void* framebuffer, AzerEGLContext* ctx) {
  return TexturePtr();
}

TexturePtr GetCurrentFramebufferTexture(AzerEGLContext* ctx) {
  EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
  if (surface == EGL_NO_SURFACE) {
    return TexturePtr();
  }

  return GetSurfaceTexture((void*)surface, ctx);
}

namespace {
void InitDeviceConfigAttr(AzerEGLContext* context, EGLint* buf, int size) {
  CHECK_GT(size, 64);
  EGLint* cur = buf;
  if (context->colorbuf_format == kBGRAn8
      || context->colorbuf_format == kBGRAn8) {
    *buf++ = EGL_RED_SIZE;
    *buf++ = 8;
    *buf++ = EGL_GREEN_SIZE;
    *buf++ = 8;
    *buf++ = EGL_BLUE_SIZE;
    *buf++ = 8;
    *buf++ = EGL_ALPHA_SIZE;
    *buf++ = 8;
  } else {
    NOTREACHED() << "unsupported data format";
  }

  if (context->depthbuf_format) {
    *buf++ = EGL_DEPTH_SIZE;
    *buf++ = 24;
    *buf++ = EGL_STENCIL_SIZE;
    *buf++ = 8;
  }
  *buf++ = EGL_SAMPLE_BUFFERS;
  *buf++ =  0;
  *buf++ = EGL_NONE;
}
}  // namespace
}  // namespace angle
}  // namespace azer
