#pragma once


#include <d3d11.h>

#include "azer/render/indices_buffer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11RenderSystem;

class D3D11IndicesBuffer : public IndicesBuffer {
 public:
  virtual ~D3D11IndicesBuffer() {
    SAFE_RELEASE(buffer_);
  }

  virtual bool Init();
  /**
   * 对于 VertexBuffer 来说，它是不分行和列的， 仅仅知道大小
   */
  virtual HardwareBufferDataPtr map(MapType flags) OVERRIDE;
  virtual void unmap() OVERRIDE;

  bool Initialized() { return NULL != buffer_;}
 private:
  D3D11IndicesBuffer(const IndicesBuffer::Options& opt, IndicesDataPtr data,
                     D3D11RenderSystem* rs)
      : IndicesBuffer(opt, data)
      , locked_(false)
      , buffer_(NULL)
      , render_system_(rs) {
  }

  bool locked_;
  ID3D11Buffer* buffer_;
  D3D11RenderSystem* render_system_;

  friend class D3D11RenderSystem;
  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11IndicesBuffer);
};
}  // namespace azer
