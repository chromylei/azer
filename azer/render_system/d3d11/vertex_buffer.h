#pragma once

#include "base/basictypes.h"
#include "azer/render/vertex_buffer.h"
#include "azer/render_system/d3d11/render_system.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11VertexBuffer : public VertexBuffer {
 public:
  virtual ~D3D11VertexBuffer() {
    SAFE_RELEASE(buffer_);
  }
  
  bool Init();

  /**
   * 对于 VertexBuffer 来说，它是不分行和列的， 仅仅知道大小
   */
  virtual LockDataPtr map(MapType flags) OVERRIDE;
  virtual void unmap() OVERRIDE;

  bool Initialized() const { return NULL != buffer_;}
 private:
  D3D11VertexBuffer(const Options &opt, const VertexDataPtr& dataptr,
                    D3D11RenderSystem* rs)
      : VertexBuffer(opt, dataptr)
      , locked_(false)
      , buffer_(NULL)
      , render_system_(rs) {
  }

  bool locked_;
  ID3D11Buffer* buffer_;
  D3D11RenderSystem* render_system_;
  friend class D3D11RenderSystem;
  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11VertexBuffer);
};

}  // namespace azer
