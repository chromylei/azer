#include "azer/ui/widget/controls/panel.h"

#include "azer/ui/widget/context.h"
#include "azer/ui/widget/theme/theme.h"
#include "azer/ui/widget/canvas.h"
#include "azer/render/skia/canvas.h"
#include "base/logging.h"

namespace azer {
namespace ui {
Panel::Panel(PanelStylePtr& style, const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent)
    , style_(style) {
  DCHECK(NULL != style_);
}

void Panel::Redraw(bool force) {
  gfx::Rect rect(0, 0, rect_.width(), rect_.height());
  Canvas* canvas = canvas_;
  canvas->begin();
  SkPaint paint;
  paint.setColor(style_->frame_color());
  if (style_->round_rect_radius()) {
    canvas->DrawRoundRect(rect, style_->round_rect_radius(), paint);
  } else {
    canvas->DrawRect(rect, paint);
  }

  canvas->FillRect(rect_, (SkColor)(style_->bgcolor()));
  
  canvas->end();
}
}  // namespace ui {
}  // namespace azer
