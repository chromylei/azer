#include "azer/render_system/d3d11/render_system.h"

#include <string>


#include "base/strings/string16.h"
#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/base/image.h"
#include "azer/ui/window/window_host.h"
#include "azer/render/render_system_enum.h"
#include "azer/render_system/d3d11/util.h"
#include "azer/render_system/d3d11/blending.h"
#include "azer/render_system/d3d11/depth_buffer.h"
#include "azer/render_system/d3d11/enum_transform.h"
#include "azer/render_system/d3d11/gpu_constants_table.h"
#include "azer/render_system/d3d11/gpu_program.h"
#include "azer/render_system/d3d11/indices_buffer.h"
#include "azer/render_system/d3d11/render_target.h"
#include "azer/render_system/d3d11/renderer.h"
#include "azer/render_system/d3d11/overlay.h"
#include "azer/render_system/d3d11/technique.h"
#include "azer/render_system/d3d11/texture.h"
#include "azer/render_system/d3d11/vertex_buffer.h"
#include "azer/render_system/d3d11/angle/angle.h"
#include "azer/render_system/d3d11/angle/glinterface.h"

namespace azer {

const StringType& D3D11RenderSystem::name_ = AZER_LITERAL("Direct3D11RenderSystem");
const StringType& D3D11RenderSystem::short_name_ = AZER_LITERAL("d3d11");

D3D11RenderSystem::D3D11RenderSystem(WindowHost* window)
    : RenderSystem(window)
    , d3d_device_(NULL)
    , d3d_context_(NULL)
    , dxgi_factory_(NULL) {
}

D3D11RenderSystem::~D3D11RenderSystem() {
  SAFE_RELEASE(dxgi_factory_);
  SAFE_RELEASE(d3d_context_);
  SAFE_RELEASE(d3d_device_);
}

bool D3D11RenderSystem::Init() {
  if (!InitD3DDevice()) {
    return false;
  }

  WindowHost* winhost = GetWindowHost();
  gfx::Rect rect = std::move(winhost->GetClientBounds());
  std::unique_ptr<D3D11SwapChain> ptr(new D3D11SwapChain(this));
  if (!ptr->Init(rect.width(), rect.height())) {
    return false;
  }

  swap_chain_.reset(ptr.release());
  GetDefaultRenderer()->Use();
  return true;
}

bool D3D11RenderSystem::Present() {
  DCHECK(swap_chain_ != NULL) << "swap_chain cannto be NULL";
  if (!swap_chain_->Present()) {
    LOG(ERROR) << " failed to Present.";

    gfx::Rect rect = std::move(win_host_->GetClientBounds());
    return swap_chain_->reset(rect.width(), rect.height());
  }

  return true;
}

bool D3D11RenderSystem::reset() {
  DCHECK(swap_chain_ != NULL);
  gfx::Rect rect = std::move(win_host_->GetClientBounds());
  return swap_chain_->reset(rect.width(), rect.height());
}

void D3D11RenderSystem::GetDriverCapability() {
  capability_.hw_transform_light_ = true;
}

const StringType& D3D11RenderSystem::name() const {
  return name_;
}

const StringType& D3D11RenderSystem::short_name() const {
  return short_name_;
}

VertexBuffer* D3D11RenderSystem::CreateVertexBuffer(
    const VertexBuffer::Options& opt, VertexData* dataptr) {
  std::unique_ptr<D3D11VertexBuffer> vertex_buffer(
      new D3D11VertexBuffer(opt, this));
  if (vertex_buffer->Init(dataptr)) {
    return vertex_buffer.release();
  } else {
    return NULL;
  }
}


IndicesBuffer* D3D11RenderSystem::CreateIndicesBuffer(
    const IndicesBuffer::Options& opt, IndicesData* dataptr) {
  std::unique_ptr<D3D11IndicesBuffer> indices_buffer(
      new D3D11IndicesBuffer(opt, this));
  if (indices_buffer->Init(dataptr)) {
    return indices_buffer.release();
  } else {
    return NULL;
  }
}

GpuProgram* D3D11RenderSystem::CreateGpuProgram(RenderPipelineStage stage,
                                                const std::string& program) {
  std::unique_ptr<GpuProgram> gpu_program;
  switch (stage) {
    case kPixelStage:
      gpu_program.reset(new D3D11PixelGpuProgram(program));
      break;
    case kGeometryStage:
      gpu_program.reset(new D3D11GeometryGpuProgram(program));
      break;
    case kVertexStage:
      CHECK(false) << "Vertex GpuProgram has its own ";
    default:
      CHECK(false) << "No such GpuProgram Type: " << (int32)stage;
      return NULL;
  }
  if (gpu_program->Init(this)) {
    return gpu_program.release();
  } else {
    return NULL;
  }
}

VertexGpuProgram* D3D11RenderSystem::CreateVertexGpuProgram(
    VertexDescPtr desc, const std::string& program) {
  std::unique_ptr<VertexGpuProgram> gpu_program(
      new D3D11VertexGpuProgram(desc, program));
  if (gpu_program->Init(this)) {
    return gpu_program.release();
  } else {
    return NULL;
  }
}

Technique* D3D11RenderSystem::CreateTechnique() {
  return new D3D11Technique(this);
}

GpuConstantsTable* D3D11RenderSystem::CreateGpuConstantsTable(
    int32 num, const GpuConstantsTable::Desc* desc) {
  std::unique_ptr<D3D11GpuConstantsTable> tableptr(
      new D3D11GpuConstantsTable(num, desc));
  if (tableptr->Init(this)) {
    return tableptr.release();
  } else {
    return NULL;
  }
}

Texture* D3D11RenderSystem::CreateTexture(const Texture::Options& opt,
                                          const Image* img) {
  const ImageDataPtr& image = img->data(0);
  Texture::Options texopt = opt;
  texopt.width = image->width();
  texopt.height = image->height();
  texopt.format = image->format();
  texopt.type = (Texture::Type)img->type();
  std::unique_ptr<D3D11Texture> tex;
  if (texopt.type == Texture::k2D) {
    tex.reset(new D3D11Texture2D(texopt, this));
  } else if (texopt.type == Texture::kCubeMap) {
    tex.reset(new D3D11TextureCubeMap(texopt, this));
  } else {
    NOTREACHED();
    return NULL;
  }
  if (tex->InitFromImage(img)) {
    return tex.release();
  } else {
    return NULL;
  }
}

Texture* D3D11RenderSystem::CreateTexture(const Texture::Options& opt) {
  if (opt.type == Texture::k2D) {
    return new D3D11Texture2D(opt, this);
  } else {
    return NULL;
  }
}

/*
RenderTarget* D3D11RenderSystem::CreateRenderTarget(const Texture::Options& opt) {
  std::unique_ptr<D3D11RenderTarget> target(new D3D11RenderTarget(opt, false, this));
  if (target->Init(this)) {
    return target.release();
  } else {
    return NULL;
  }
}

DepthBuffer* D3D11RenderSystem::CreateDepthBuffer(const Texture::Options& opt) {
  DCHECK(opt.format == kDepth24Stencil8);
  DCHECK(opt.target & Texture::kDepthStencil);
  std::unique_ptr<D3D11DepthBuffer> depthbuffer(new D3D11DepthBuffer(opt, this));
  if (depthbuffer->Init()) {
    return depthbuffer.release();
  } else {
    return false;
  }
}
*/

Overlay* D3D11RenderSystem::CreateOverlay(const gfx::RectF& rect) {
  std::unique_ptr<D3D11Overlay> surface_ptr(new D3D11Overlay(rect, this));
  if (surface_ptr->Init(this)) {
    return surface_ptr.release();
  } else {
    return NULL;
  }
}

Blending* D3D11RenderSystem::CreateBlending(const Blending::Desc& desc) {
  std::unique_ptr<D3D11Blending> blending(new D3D11Blending(desc, this));
  if (blending->Init()) {
    return blending.release();
  } else {
    return NULL;
  }
}

Renderer* D3D11RenderSystem::CreateRenderer(const Texture::Options& opt) {
  DCHECK(GetDefaultRenderer() != NULL);
  std::unique_ptr<D3D11Renderer> renderer(new D3D11Renderer(
      ((D3D11Renderer*)GetDefaultRenderer())->GetContext(), this));
  if (renderer->Init(opt)) {
    return renderer.release();
  } else {
    return NULL;
  }
}

Renderer* D3D11RenderSystem::CreateDeferredRenderer(const Texture::Options& opt) {
  DCHECK(GetDevice() != NULL);
  ID3D11DeviceContext* context = NULL;
  HRESULT hr = GetDevice()->CreateDeferredContext(0, &context);
  if (FAILED(hr)) {
    LOG(ERROR) << "Failed to CreateDeferredContext";
    return NULL;
  }

  std::unique_ptr<D3D11Renderer> renderer(new D3D11Renderer(context, this));
  if (renderer->Init(opt)) {
    return renderer.release();
  } else {
    return NULL;
  }
}

AzerEGLInterface* D3D11RenderSystem::GetEGLInterface() {
  return new ANGLEGLInterface(this);
}

bool D3D11RenderSystem::InitD3DDevice() {
  HRESULT hr;
  // Create our SwapChain
  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  hr = D3D11CreateDevice(NULL,
                         D3D_DRIVER_TYPE_HARDWARE,
                         NULL,
                         NULL,
                         featureLevels, arraysize(featureLevels),
                         D3D11_SDK_VERSION,
                         &d3d_device_,
                         &feature_level_,
                         &d3d_context_);
  if (FAILED(hr)) {
    return false;
  }

  IDXGIDevice *dxgiDevice = NULL; 
  hr = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
  if (FAILED(hr)) {
    LOG(ERROR) << "Failed to get Interface: IDXGIDevice";
    return false;
  }

  hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter_);
  if (FAILED(hr)) {
    LOG(ERROR) << "Failed to get Interface: IDXGIAdapter";
    return false;
  }

  hr = dxgi_adapter_->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory_);
  if (FAILED(hr)) {
    LOG(ERROR) << "Failed to get Interface: IDXGIFactory";
    return false;
  }

  d3d_context_->AddRef();
  return true;
}
}  // namespace azer

