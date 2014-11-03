#include "azer/ui/widget/context.h"

#include <memory>

#include "base/logging.h"
#include "azer/render/glcontext.h"
#include "azer/render/skia/context.h"
#include "azer/render/skia/skia.h"
#include "azer/render/skia/canvas.h"
#include "azer/ui/window/window_host.h"
#include "azer/ui/widget/root_window.h"
#include "azer/ui/widget/theme.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/canvas.h"

namespace azer {
namespace ui {
Context::Context(RenderSystem* rs)
    : render_system_(rs)
    , theme_(NULL)
    , canvas_(NULL) {
}

Context::~Context() {
  if (canvas_) {
    delete canvas_;
  }

  if (theme_) {
    delete theme_;
  }
}

Context* Context::Create(RenderSystem* rs) {
  std::unique_ptr<Context> ptr(new Context(rs));
  if (ptr->Init()) {
    return ptr.release();
  } else {
    return NULL;
  }
}

bool Context::Init() {
  DCHECK(NULL == sk_context_.get());
  sk_context_.reset(skia::Init());
  if (!sk_context_) {
    LOG(ERROR) << "failed to initialize skia.";
    return false;
  }

  gfx::RectF rect(-1.0f, -1.0f, 2.0f, 2.0f);
  overlay_.reset(render_system_->CreateOverlay(rect));
  if (!overlay_.get()) {
    LOG(ERROR) << "failed to create overlay";
    return false;
  }
  overlay_effect_.reset(CreateOverlayEffect());
  overlay_->SetEffect(overlay_effect_);

  WindowHost* host = render_system_->GetWindowHost();
  int width = host->GetMetrics().width;
  int height = host->GetMetrics().height;
  ccanvas_ = sk_context_->CreateCanvas(width, height);
  if (!ccanvas_.get()) {
    LOG(ERROR) << "Failed to init canvas.";
    return false;
  }

  root_.reset(new RootWindow(this));
  if (!root_->Init()) {
    return false;
  }

  canvas_ = gfx::Canvas::CreateCanvasWithoutScaling(ccanvas_->GetSkCanvas(), 1.0f);
  return true;
}

void Context::SetOverlayEffect(EffectPtr& ptr) {
  OverlayEffect* effect = (OverlayEffect*)ptr.get();
  effect->SetTexture(ccanvas_->GetTexture());
}

void Context::Render(azer::Renderer* renderer) {
  DCHECK(root_.get() != NULL);
  DCHECK(overlay_.get() != NULL);
  DCHECK(sk_context_.get() != NULL);
  root_->Redraw(false);
  sk_context_->wait();
  SetOverlayEffect(overlay_->GetEffect());
  overlay_->Render(renderer);
}

Effect* Context::CreateOverlayEffect() {
  return overlay_->CreateDefaultEffect();
}
}  // namespace ui
}  // namespace azer
