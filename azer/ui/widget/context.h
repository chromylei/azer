#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/overlay.h"
#include "azer/render/effect.h"
#include "base/logging.h"

namespace azer {

namespace skia {
class Context;
}  // namespace skia

class RootWindow;

namespace ui {
class AZER_EXPORT Context {
 public:
  virtual ~Context() {}
  static Context* Create(RenderSystem* rs);

  virtual void Render(azer::Renderer* renderer);
  OverlayPtr& GetOverlay();
  EffectPtr& GetOverlayEffect();
  RootWindow* GetRootWindow();
 protected:
  Context(RenderSystem* rs);
  virtual bool Init();

  // override the function to change render effect
  virtual OverlayEffect* CreateOverlayEffect();
  virtual void SetOverlayEffect(EffectPtr& ptr);

  RenderSystem* render_system_;
  OverlayPtr overlay_;
  skia::Context sk_context_;
  std::unique_ptr<RootWindow> root_;
 private:
  DISALLOW_COPY_AND_ASSIGN(Context);
};

inline OverlayPtr& Context::GetOverlay() {
  DCHECK(NULL != overlay_.get());
  return overlay_;
}

inline EffectPtr& Context::GetOverlayEffect() {
  DCHECK(NULL != overlay_.get());
  return overlay_.GetEffect();
}

inline RootWindow* Context::GetRootWindow() {
  DCHECK(root_.get() != NULL);
  return root_.get();
}
}  // namespace ui
}  // namespace azer
