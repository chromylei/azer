#include "azer/ui/widget/theme.h"

#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {
namespace ui {

Theme* Theme::theme_ = NULL;
Theme* Theme::Current() {
  return theme_;
}

void Theme::SetTheme(Theme* theme) {
  theme_ = theme;
}

Theme::Theme()
    : label_font_(NULL) {
}

DefaultTheme::~DefaultTheme() {
  if (label_font_) delete label_font_;
}

bool DefaultTheme::Init() {
  gfx::FontList::SetDefaultFontDescription("Droid Sans serif, Sans serif, 14px");
  label_font_ = new gfx::FontList("Courier New, 14px");
  return true;
}
}  // namespace ui
}  // namespace azer

