#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/texture.h"

namespace azer {

class AZER_EXPORT DepthBuffer {
 public:
  virtual ~DepthBuffer() {};

  virtual void Enable(bool) = 0;
  virtual bool IsEnabled() = 0;
  virtual void EnableWrite(bool) = 0;
  virtual void SetBias(float val) = 0;
  virtual void SetDepthCompareFunc(CompareFunc::Type func) = 0;

  // store and restore state of DepthBuffer and Stencil Buffer
  virtual void PushState() = 0;
  virtual void PopState() = 0;

  TexturePtr& GetTexture() { return texture_;}
  
  enum ClearFlag {
    kClearDepth    = 0x0001,
    kClearStencil  = 0x0002,
    kClearAll      = (kClearDepth | kClearStencil),
  };
protected:
  DepthBuffer(const Texture::Options& opt)
      : options_(opt) {
  }

  TexturePtr texture_;
  const Texture::Options& options_;
  DISALLOW_COPY_AND_ASSIGN(DepthBuffer);
};

typedef std::shared_ptr<DepthBuffer> DepthBufferPtr;

class ScopedDepthBufferState {
 public:
  ScopedDepthBufferState(DepthBuffer* buffer)
      : depth_buffer_(buffer) {
    depth_buffer_->PushState();
  }
  ~ScopedDepthBufferState() {
    depth_buffer_->PopState();
  }
 private:
  DepthBuffer* depth_buffer_;
};
}  // namespace azer
