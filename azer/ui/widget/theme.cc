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

Theme::Theme() {
}

DefaultTheme::~DefaultTheme() {
}

bool DefaultTheme::Init() {
  gfx::FontList::SetDefaultFontDescription("Droid Sans serif, Sans serif, 14px");
  return true;
}
}  // namespace ui
}  // namespace azer

