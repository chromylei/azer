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

  virtual HardwareBufferDataPtr map(MapType flags) OVERRIDE;
  virtual void unmap() OVERRIDE;

  bool Initialized() { return NULL != buffer_;}
 private:
  D3D11IndicesBuffer(const IndicesBuffer::Options& opt, D3D11RenderSystem* rs)
      : IndicesBuffer(opt)
      , locked_(false)
      , buffer_(NULL)
      , render_system_(rs) {
  }

  bool Init(const IndicesData* data);
  bool locked_;
  ID3D11Buffer* buffer_;
  D3D11RenderSystem* render_system_;

  friend class D3D11RenderSystem;
  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11IndicesBuffer);
};
}  // namespace azer
