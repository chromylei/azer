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

namespace azer {

const StringType& D3D11RenderSystem::name_ = AZER_LITERAL("Direct3D11RenderSystem");
const StringType& D3D11RenderSystem::short_name_ = AZER_LITERAL("d3d11");

D3D11RenderSystem::D3D11RenderSystem()
    : swap_chain_(NULL)
    , d3d_device_(NULL) {
}

D3D11RenderSystem::~D3D11RenderSystem() {
  SAFE_RELEASE(swap_chain_);
  SAFE_RELEASE(d3d_device_);
}


bool D3D11RenderSystem::Init(WindowHost* window) {
  DCHECK(NULL == swap_chain_);
  DCHECK(NULL == d3d_device_);
  using ::base::HRMessage;
  HRESULT hr;

  //Describe our Buffer
  DXGI_MODE_DESC bufferDesc;
  ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
  bufferDesc.Width = window->GetMetrics().width;
  bufferDesc.Height = window->GetMetrics().height;
  bufferDesc.RefreshRate.Numerator = 60;
  bufferDesc.RefreshRate.Denominator = 1;
  bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  
  //Describe our SwapChain
  DXGI_SWAP_CHAIN_DESC swapChainDesc; 
  
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapChainDesc.BufferDesc = bufferDesc;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 1;
  swapChainDesc.OutputWindow = (HWND)window->Handle();
  swapChainDesc.Windowed = !window->GetMetrics().fullscreen;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  //Create our SwapChain
  ID3D11DeviceContext* d3d_context;
  hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
                                     NULL, NULL, NULL, NULL,
                                     D3D11_SDK_VERSION, &swapChainDesc,
                                     &swap_chain_, &d3d_device_, NULL,
                                     &d3d_context);
  HRESULT_HANDLE(hr, ERROR, "Failed to create D3D11 and Swapchain ");

  azer::Texture::Options o;
  o.width = window->GetMetrics().width;
  o.height = window->GetMetrics().height;
  renderer_.reset(new D3D11Renderer(d3d_context, this));
  if (!renderer_->Init(o)) {
    return false;
  }

  renderer_->Use();
  return true;
}

void D3D11RenderSystem::Present() {
  DCHECK(swap_chain_ != NULL) << "swap_chain cannto be NULL";
  swap_chain_->Present(0, 0);
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
  if (img->depth() == 1u) {
    const ImageDataPtr& image = img->data(0);
    Texture::Options texopt = opt;
    texopt.width = image->width();
    texopt.height = image->height();
    texopt.format = image->format();
    std::unique_ptr<D3D11Texture2D> tex(new D3D11Texture2D(texopt, this));
    if (tex->InitFromData(image->data(), image->data_size())) {
      return tex.release();
    } else {
      return NULL;
    }
  } else {
    NOTREACHED();
    return NULL;
  }
}

Texture* D3D11RenderSystem::CreateTexture(const Texture::Options& opt) {
  return new D3D11Texture2D(opt, this);
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
  if (renderer_->Init(opt)) {
    return renderer.release();
  } else {
    return NULL;
  }
}

}  // namespace azer

