
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

RendererPtr CreateRenderer(RenderSystem* rs, Context* ctx) {
  Texture::Options options;
  options.width = ctx->width;
  options.height = ctx->height;
  options.format = azer::kBGRAn8;
  options.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  return RendererPtr(rs->CreateRenderer(options));
}

TexturePtr CreatePBufferTexture(RenderSystem* rs, Context* ctx) {
  D3D11RenderSystem* d3d11rs = (D3D11RenderSystem*)rs;
  Texture::Options options;
  options.width = ctx->width;
  options.height = ctx->height;
  options.format = azer::kRGBAn8;
  options.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  options.sampler.sample_level = 1;
  options.usage = GraphicBuffer::kDefault;

  std::unique_ptr<D3D11Texture2D> tex(new D3D11Texture2D(options, d3d11rs));
  if (!tex->Init(NULL, 0)) {
    return TexturePtr();
  }

  ID3D11Resource* resource = tex->GetResource();
  ID3D11Texture2D* shared_tex = NULL;
  ID3D11Device* d3d_device = d3d11rs->GetDevice();
  HRESULT hr = d3d_device->OpenSharedResource(resource, __uuidof(ID3D11Texture2D),
    (void**)&shared_tex);
  if (FAILED(hr)) {
    SAFE_RELEASE(shared_tex);
    TexturePtr();
  }
  return TexturePtr(tex.release());
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

  ctx->renderer = CreateRenderer(rs, ctx);
  ctx->tex = ctx->renderer->GetRenderTarget()->GetTexture();
  ctx->tex = CreatePBufferTexture(rs, ctx);
  D3D11Texture* texture = (D3D11Texture*)ctx->tex.get();

  EGLint surfaceAttribList[] =  {
    EGL_WIDTH,   -1,
    EGL_HEIGHT,  -1,
    EGL_NONE, EGL_NONE
  };
  surfaceAttribList[1] = ctx->width;
  surfaceAttribList[3] = ctx->height;
  EGLClientBuffer egl_buffer = (EGLClientBuffer)(texture->GetResource());
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

  ID3D11Texture2D* shared_tex = NULL;
  ID3D11Device* d3d_device = rs->GetDevice();
  HRESULT hr = d3d_device->OpenSharedResource(resource, __uuidof(ID3D11Texture2D),
                                              (void**)&shared_tex);
  if (FAILED(hr)) {
    SAFE_RELEASE(shared_tex);
    TexturePtr();
  }

  D3D11_TEXTURE2D_DESC desc;
  shared_tex->GetDesc(&desc);
  Texture::Options opt;
  opt.width = desc.Width;
  opt.height = desc.Height;
  opt.format = TranslateD3DFormat(desc.Format);
  TexturePtr ptr(new D3D11Texture2DShared(opt, shared_tex, rs));
  // SAFE_RELEASE(resource);
  return ptr;
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
