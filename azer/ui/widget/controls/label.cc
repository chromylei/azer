#include "azer/ui/widget/controls/label.h"

#include "azer/ui/widget/context.h"
#include "azer/ui/widget/widget.h"
#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/theme.h"
#include "azer/ui/widget/painter.h"

namespace azer {
namespace ui {
Label::Label(const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent) {
}

Label::Label(const StringType& str, const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent)
    , text_(str) {
}

void Label::Redraw(bool force) {
  const int32 default_label_draw_flags = gfx::Canvas::TEXT_ALIGN_LEFT
      | gfx::Canvas::NO_ELLIPSIS
      | gfx::Canvas::NO_SUBPIXEL_RENDERING;

  gfx::Rect rect(0, 0, rect_.width(), rect_.height());
  Canvas* canvas = canvas_;
  Theme* theme = Theme::Current();
  const LabelStyle* style = theme->label_style();
  const TextStyle* text_style = style->text_style();
  const FrameStyle* frame_style = style->frame_style();
  Painter painter;
  painter.begin(canvas);
  painter.DrawLabelText(rect, text_, text_style, frame_style);
  painter.DrawFrame(rect, style->frame_style());
  painter.end();
  canvas->end();
}
}  // namespace ui
}  // namespace azer

