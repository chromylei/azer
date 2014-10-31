#include "azer/ui/widget/context.h"

#include <memory>

#include "base/logging.h"
#include "azer/render/glcontext.h"
#include "azer/render/skia/context.h"
#include "azer/ui/widget/root_window.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {
Context::Context(RenderSystem* rs)
    : render_system_(rs)
    , skia_context_(NULL) {
}

Context* Context::Create(RenderSystem* rs) {
  std::unique_ptr<Context> ptr(new Context(rs));
  if (ptr->Init()) {
    return ptr->release();
  } else {
    return NULL;
  }
}

bool Context::Init() {
  DCHECK(NULL == skia_context_);
  skia_context_ = skia::Init();
  if (!skia_context_) {
    LOG(ERROR) << "failed to initialize skia.";
    return false;
  }

  gfx::RectF rect(-1.0f, -1.0f, 2.0f, 2.0f);
  overlay_.reset(render_system_->CreateOverlay(rect));
  if (!overlay_.get()) {
    LOG(ERROR) << "failed to create overlay";
    return false;
  }

  root_.reset(RootWindow::Create(render_system_));
  return true;
}

void Context::SetOverlayEffect(EffectPtr& ptr) {
  ptr->SetTexture();
}

void Context::Render(azer::Renderer* renderer) {
  DCHECK(overlay_.get() != NULL);
  UseEffect(overlay_->GetEffect());
  overlay_->Render(renderer);
}

OverlayEffect* Context::CreateOverlayEffect() {
  return overlay_->CreateDefaultEffect();
}
}  // namespace ui
}  // namespace azer
