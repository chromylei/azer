#include "azer/ui/widget/controls/label.h"

#include "azer/ui/widget/context.h"
#include "azer/ui/widget/widget.h"
#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/theme.h"

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
  canvas->begin();
  Theme* theme = Theme::Current();
  const LabelStyle* style = theme->label_style();
  canvas->FillRect(rect, SkColorSetARGBInline(64, 64, 64, 128));
  canvas->DrawStringRectWithFlags(text,
                                  *style->text_style()->font_list(),
                                  style->text_style()->fgcolor(),
                                  rect,
                                  default_label_draw_flags);
  canvas->FillRect(rect, (SkColor)style->frame_style()->bgcolor());

  const BorderStyle* bstyle = style->frame_style()->border_style();
  Painter painter;
  painter.begin(canvas);
  painter.DrawFrame(rect, bstyle);
  painter.end();
  canvas->end();
}
}  // namespace ui
}  // namespace azer

