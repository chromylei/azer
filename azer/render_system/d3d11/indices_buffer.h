#pragma once


#include <d3d11.h>

#include "azer/render/indices_buffer.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11RenderSystem;

class D3D11IndicesBuffer : public IndicesBuffer {
 public:
  virtual ~D3D11IndicesBuffer() {
    SAFE_RELEASE(indices_buffer_);
  }

  virtual bool Init();
  virtual IndicesData* Lock() {return NULL;}
  virtual void Unlock() {}

  bool Initialized() { return NULL != indices_buffer_;}
 private:
  D3D11IndicesBuffer(const IndicesBuffer::Options& opt, IndicesDataPtr data,
                     D3D11RenderSystem* rs)
      : IndicesBuffer(opt, data)
      , render_system_(rs)
      , indices_buffer_(NULL) {
  }

  D3D11RenderSystem* render_system_;
  ID3D11Buffer* indices_buffer_;

  friend class D3D11RenderSystem;
  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11IndicesBuffer);
};
}  // namespace azer
