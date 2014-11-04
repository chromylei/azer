#include "azer/ui/widget/painter.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/style.h"

namespace azer {
namespace ui {

Painter::Painter()
    : canvas_(NULL) {
}

void Painter::begin(Canvas* canvas) {
  DCHECK(canvas_ == NULL);
  canvas_ = canvas;
  canvas_->begin();
}

void Painter::end() {
  DCHECK(canvas_ != NULL);
  canvas_->end();
  canvas_ = NULL;
}

void Painter::DrawFrame(const gfx::Rect& rect, const BorderStyle* style) {
  Canvas* canvas = canvas_;
  if (style->style() == BorderStyle::kSingleLine) {
    canvas->DrawLine(rect.origin(), rect.top_right(), bstyle->color());
    canvas->DrawLine(rect.origin(), rect.bottom_left(), bstyle->color());
    canvas->DrawLine(rect.bottom_right(), rect.top_right(), bstyle->color());
    canvas->DrawLine(rect.bottom_right(), rect.bottom_left(), bstyle->color());
  } else {
  }
}

}  // namespace ui
}  // namespace azer
