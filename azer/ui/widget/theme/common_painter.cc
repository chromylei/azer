#include "azer/ui/widget/theme/common_painter.h"
#include "azer/ui/widget/canvas.h"

namespace azer {
namespace ui {
BackgroundPainter::BackgroundPainter(uint32 bgcolor_argb)
    : color_(bgcolor_argb) {
}

BackgroundPainter::~BackgroundPainter() {
}

void BackgroundPainter::Paint(Widget* widget, const gfx::Rect& rect,
                              Context* context) {
  Canvas* canvas = widget->GetCanvas();
  canvas->FillRect(rect, (SkColor)color_);
}

TextPainter::TextPainter(uint32 bgcolor_argb, const std::string& fontstr)
    : color_(bgcolor_argb)
    , fontlist_(fontstr) {
}

TextPainter::~TextPainter() {
}

void TextPainter::Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) {
  TextContext* context = (TextContext*)ctx;
  Canvas* canvas = widget->GetCanvas();
  const int32 default_label_draw_flags = gfx::Canvas::TEXT_ALIGN_LEFT
      | gfx::Canvas::NO_ELLIPSIS
      | gfx::Canvas::NO_SUBPIXEL_RENDERING;
  canvas->DrawStringRectWithFlags(context->text(),
                                  fontlist_,
                                  color_,
                                  rect,
                                  default_label_draw_flags);
}
}  // namespace ui
}  // namespace azer
