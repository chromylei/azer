#include "azer/ui/widget/label.h"

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
  uint32 bcolor = (uint32)SkColorSetARGBInline(0, 0, 0, 0);
  uint32 fcolor = (uint32)SkColorSetARGBInline(255, 192, 192, 192);
  canvas->FillRect(rect, SkColorSetARGBInline(64, 64, 64, 128));
  canvas->DrawStringRectWithFlags(text_,
                                  theme->label_font(),
                                  fcolor,
                                  rect,
                                  default_label_draw_flags);
  canvas->FillRect(rect_, (SkColor)bcolor);
  canvas->end();
}
}  // namespace ui
}  // namespace azer

