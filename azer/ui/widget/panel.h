#pragma once

#include "base/basictypes.h"
#include "azer/render/render_export.h"
#include "azer/ui/widget.h"
#include "azer/ui/gfx/gfx.h"

namespace azer {
class RenderSystem;

class AZER_EXPORT Panel : public azer::Widget {
 public:
  virtual ~Panel() {}
  void Add(azer::Widget* child);

  /**
   * create a panel on window
   * note: rect is coordinate on parent
   */
  static Panel* Create(const gfx::Rect& rect, RenderSystem* render_system,
                       azer::Widget* parent);

  virtual void Render(gfx::Canvas* canvas);

  // ARGB
  void SetFillColor(uint32 color) { color_ = color;}
  uint32 GetFillColor() const { return color_;
 private:
  Pannel();

  RenderSystem* render_system_;
  CanvasPtr canvas_;
  uint32 color_;
  DISALLOW_COPY_AND_ASSIGN(Panel);
};
}  // namespace azer
