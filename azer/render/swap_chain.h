#pragma once

#include <memory>

#include "base/logging.h"
#include "azer/base/render_export.h"
#include "azer/render/depth_buffer.h"

namespace azer {

class WindowHost;

class AZER_EXPORT SwapChain {
 public:
  SwapChain(WindowHost* host) : win_host_(host) {}
  virtual ~SwapChain() {}

  virtual bool recreate() = 0;

  virtual bool resize(int width, int height) = 0;
  virtual void Present() = 0;

  TexturePtr& GetRenderTargetTexture() {
    return render_target_tex_;
  }

  DepthBufferPtr& getDepthBuffer() { return depth_buffer_;}
 protected:
  WindowHost* win_host_;
  DepthBufferPtr depth_buffer_;
  int width_, height_;
  TexturePtr render_target_tex_;
  DISALLOW_COPY_AND_ASSIGN(SwapChain);
};

typedef std::shared_ptr<SwapChain> SwapChainPtr;
}  // namespace azer

