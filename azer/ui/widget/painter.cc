#include "azer/ui/widget/painter.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/style.h"
#include "azer/ui/widget/controls/label.h"

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

void Painter::DrawFrame(const gfx::Rect& rect, const FrameStyle* style) {
  Canvas* canvas = canvas_;
  const BorderStyle* bstyle = style->border_style();
  if (bstyle->style() == BorderStyle::kSingleLine) {
    canvas->DrawLine(rect.origin(), rect.top_right(), bstyle->color());
    canvas->DrawLine(rect.origin(), rect.bottom_left(), bstyle->color());
    canvas->DrawLine(rect.bottom_right(), rect.top_right(), bstyle->color());
    canvas->DrawLine(rect.bottom_right(), rect.bottom_left(), bstyle->color());
  } else {
  }

  canvas->FillRect(rect, (SkColor)style->bgcolor());
}

void Painter::DrawLabelText(const gfx::Rect& rect, const StringType& text,
                            const TextStyle* style, const FrameStyle* fstyle) {
  const int32 default_label_draw_flags = gfx::Canvas::TEXT_ALIGN_LEFT
      | gfx::Canvas::NO_ELLIPSIS
      | gfx::Canvas::NO_SUBPIXEL_RENDERING;
  
  gfx::Rect rc;
  if (fstyle) {
    const Margin& margin = fstyle->margin();
    const Margin& padding = fstyle->padding();

    int top = margin.top + padding.top;
    int left = margin.left + padding.left;
    int right = margin.right + padding.right;
    int bottom = margin.bottom + padding.bottom;
    rc = gfx::Rect(rect.x() + left, rect.y() + top,
                   rect.width() - left - right,
                   rect.height() - top - bottom);
  } else {
    rc = rect;
  }
                           
  Canvas* canvas = canvas_;
  canvas->DrawStringRectWithFlags(text,
                                  *style->font_list(),
                                  style->fgcolor(),
                                  rc,
                                  default_label_draw_flags);
}
}  // namespace ui
}  // namespace azer
