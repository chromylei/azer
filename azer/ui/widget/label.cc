#include "azer/ui/widget/label.h"

#include "azer/ui/widget/theme.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {

Label* Label::Create(const gfx::Rect& rect, Window* parent) {
  std::unique_ptr<Label> contrl(new Label(rect, parent));
  if (contrl->Init()) {
    return contrl.release();
  } else {
    return NULL;
  }
}

Label::Label(const gfx::Rect& rc, Window* parent)
    : ControlWidget(rc, parent)
    , font_color_((uint32)SK_ColorWHITE) {
  fill_color_ = Theme::kDefaultLabelFillColor;
}

bool Label::Init() {
  return true;
}

void Label::SetText(const base::string16& str) {
  text_ = str;
}

void Label::Append(const base::string16& str) {
  text_.append(str);
}

void Label::Redraw(gfx::Canvas* canvas) {
  ThemePtr theme_ptr = Theme::GetTheme();
  canvas->DrawStringRectWithFlags(text_,
                                  theme_ptr->LabelFont(),
                                  GetFontColor(),
                                  rect_,
                                  theme_ptr->LabelFontDrawFlags());
  canvas->FillRect(rect_, (SkColor)fill_color_);
}
}  // namespace azer
