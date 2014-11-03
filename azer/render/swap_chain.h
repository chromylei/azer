#pragma once

#include <memory>

#include "base/logging.h"
#include "azer/base/render_export.h"
#include "azer/render/renderer.h"
#include "azer/render/render_target.h"
#include "azer/render/depth_buffer.h"
#include "azer/render/texture.h"

namespace azer {

class WindowHost;

class AZER_EXPORT SwapChain {
 public:
  SwapChain() {}
  virtual ~SwapChain() {}

  virtual bool resize(int width, int height) = 0;
  virtual bool reset(int width, int height) = 0;
  virtual bool Present() = 0;

  RendererPtr& GetRenderer() { return renderer_;}
  DepthBufferPtr& GetDepthBuffer() { return depth_buffer_;}
  RenderTargetPtr& GetRenderTarget() { return render_target_;}
 protected:
  RendererPtr renderer_;
  DepthBufferPtr depth_buffer_;
  RenderTargetPtr render_target_;
  DISALLOW_COPY_AND_ASSIGN(SwapChain);
};

typedef std::shared_ptr<SwapChain> SwapChainPtr;
}  // namespace azer

