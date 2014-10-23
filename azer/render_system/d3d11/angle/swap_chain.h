#pragma once

#include "base/logging.h"
#include "libGLESv2/renderer/d3d11/SwapChain11.h"

namespace azer {
/**
 * 由于 azer::RenderSystem 已经完成了 swapchain 的创建
 * 因此此处完成的功能全部使用 azer:: 来完成
 */
class AzerSwapChain : public SwapChain11 {
 public:
  AzerSwapChain();
  ~AzerSwapChain();
 private:
  DISALLOW_COPY_AND_ASSIGN(AzerSwapChain);
};
}  // namespace azer
