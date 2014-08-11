#pragma once

#include "azer/base/render_export.h"
#include "azer/render/texture.h"
#include "azer/render/render_system.h"
#include "azer/render/render_target.h"

namespace azer {
/*
class AZER_EXPORT ScopedRenderTarget {
 public:
  ScopedRenderTarget(RenderSystem* rs) {
    render_system_ = rs;
    depth_buffer_ = rs->CurrentDepthBuffer();
    default_rtarget_ = rs->CurrentRenderTarget();
  }

  ~ScopedRenderTarget() {
    render_system_->SetRenderTarget(1, default_rtarget_, depth_buffer_);
  }
 private:
  RenderSystem* render_system_;
  RenderTarget* default_rtarget_;
  DepthBuffer* depth_buffer_;
};
*/
}  // namespace azer
