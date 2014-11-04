#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/render/overlay.h"
#include "azer/render/effect.h"
#include "azer/render/blending.h"
#include "azer/render/skia/context.h"
#include "azer/render/skia/canvas.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget_host.h"
#include "base/logging.h"

namespace gfx {
class Canvas;
}  // namespace gfx

namespace azer {

namespace skia {
class Context;
class Canvas;
}  // namespace skia


namespace ui {
class Theme;
class WidgetHost;

class AZER_WIDGET_EXPORT Context {
 public:
  virtual ~Context();
  static Context* Create(RenderSystem* rs);

  /**
   * Usually, RenderLoop Look like there
   * OnRender() {
   *   context->Draw();
   *   render scene
   *   context->Render(renderer);
   *   render_system_->Present();
   * }
   * because, glFinish in context->Draw()
   * so, put the code as many as posible before it
   */
  void Draw();
  void Render(azer::Renderer* renderer);
  
  OverlayPtr& GetOverlay();
  EffectPtr& GetOverlayEffect();
  WidgetHost* GetWidgetHost();

  Theme* GetTheme() { return theme_;}

  skia::Canvas* GetSkCanvas();
  gfx::Canvas* GetCanvas();
 protected:
  Context(RenderSystem* rs);
  virtual bool Init();

  // override the function to change render effect
  virtual Effect* CreateOverlayEffect();
  virtual void SetOverlayEffect(EffectPtr& ptr);

  RenderSystem* render_system_;
  OverlayPtr overlay_;
  EffectPtr overlay_effect_;
  BlendingPtr blending_;
  std::unique_ptr<skia::Context> sk_context_;
  std::unique_ptr<WidgetHost> root_;
  skia::CanvasPtr ccanvas_;
  gfx::Canvas* canvas_;
  Theme* theme_;
 private:
  DISALLOW_COPY_AND_ASSIGN(Context);
};

inline skia::Canvas* Context::GetSkCanvas() {
  DCHECK(NULL != ccanvas_.get());
  return ccanvas_.get();
}

inline gfx::Canvas* Context::GetCanvas() {
  DCHECK(NULL != canvas_);
  return canvas_;
}

inline OverlayPtr& Context::GetOverlay() {
  DCHECK(NULL != overlay_.get());
  return overlay_;
}

inline EffectPtr& Context::GetOverlayEffect() {
  DCHECK(NULL != overlay_.get());
  return overlay_->GetEffect();
}

inline WidgetHost* Context::GetWidgetHost() {
  DCHECK(root_.get() != NULL);
  return root_.get();
}
}  // namespace ui
}  // namespace azer
