#pragma once

#include <string>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/render/render_capability.h"
#include "azer/render/renderer.h"
#include "azer/render/vertex_buffer.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
class Overlay;
class WindowHost;
class Technique;
class VertexBuffer;
class RenderTarget;
class DepthRenderTarget;
class Image;

typedef std::shared_ptr<IndicesData> IndicesDataPtr;

class AZER_EXPORT RenderSystem {
 public:
  RenderSystem() : win_host_(NULL) { }

  virtual ~RenderSystem() {}
  virtual const StringType& name() const = 0;
  virtual const StringType& short_name() const = 0;

  static RenderSystem* InitRenderSystem(const ::base::FilePath& path,
                                        WindowHost* win);
  static void Release();
  static RenderSystem* Current();
  virtual bool Init(WindowHost* window) = 0;

  virtual Renderer* CreateRenderer(const Texture::Options& opt) = 0;
  virtual Renderer* CreateDeferredRenderer(const Texture::Options& opt) = 0;
  virtual Blending* CreateBlending(const Blending::Desc& desc) = 0;
  virtual Technique* CreateTechnique() = 0;

  /**
   * Create Vertex Buffer
   */
  virtual VertexBuffer* CreateVertexBuffer(const VertexBuffer::Options& opt,
                                           VertexDataPtr) = 0;
  virtual IndicesBuffer* CreateIndicesBuffer(const IndicesBuffer::Options& opt,
                                             IndicesDataPtr) = 0;
  virtual GpuConstantsTable* CreateGpuConstantsTable(
      int32 num, const GpuConstantsTable::Desc* desc) = 0;

  // textures functions
  virtual Texture* CreateTexture(const Texture::Options& opt) = 0;
  virtual Texture* CreateTexture(const Texture::Options& opt, const Image* image) = 0;
  virtual Texture* CreateTextureFromFile(const Texture::Type type,
                                         const ::base::FilePath& path) = 0;
  // virtual RenderTarget* CreateRenderTarget(const Texture::Options& opt) = 0;
  // virtual DepthBuffer* CreateDepthBuffer(const Texture::Options& opt) = 0;  

  // create GpuProgram
  // Vertex Gpu Program need to help check "Vertex Layout"
  //
  virtual VertexGpuProgram* CreateVertexGpuProgram(VertexDescPtr desc,
                                                   const std::string& program) = 0;
  virtual GpuProgram* CreateGpuProgram(RenderPipelineStage stage,
                                       const std::string& program) = 0;
  virtual Overlay* CreateOverlay(const gfx::RectF& rect) = 0;

  virtual void Present() = 0;

  const RenderSystemCapability& capability() const {
    return capability_;
  }

  WindowHost* GetRenderWindowHost() const { return win_host_;}
  static const int32 kMaxRenderTarget = 256;

  Renderer* GetDefaultRenderer() {
    DCHECK(renderer_.get() != NULL);
    return renderer_.get();
  }
 protected:
  RenderSystemCapability capability_;
  WindowHost* win_host_;

  std::unique_ptr<Renderer> renderer_;
  friend class AutoRenderSystemInit;
  DISALLOW_COPY_AND_ASSIGN(RenderSystem);
};

}  // namespace azer

