#include "azer/ui/widget/theme.h"

#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"
#include "azer/ui/widget/canvas.h"

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

// class DefaultTheme
DefaultTheme::DefaultTheme()
    : alpha_(128) {
}

DefaultTheme::~DefaultTheme() {
}

bool DefaultTheme::Init() {
  gfx::FontList::SetDefaultFontDescription("Droid Sans serif, Sans serif, 14px");
  InitLabelStyle();
  return true;
}

void DefaultTheme::InitLabelStyle() {
  FrameStyle* fstyle = label_style()->frame_style();
  fstyle->set_bgcolor((uint32)SkColorSetARGB(alpha(), 1, 1, 1));
  fstyle->set_padding(Margin(5));
  fstyle->set_margin(Margin(5));

  BorderStyle* bstyle = fstyle->border_style();
  bstyle->set_color((uint32)SkColorSetARGB(alpha(), 0x0b, 0x0b, 0x0b));
}

void DefaultTheme::InitTooltipStyle() {
  FrameStyle* fstyle = tooltip_style()->frame_style();
  fstyle->set_bgcolor((uint32)SkColorSetARGB(alpha(), 192, 192, 192));
  fstyle->set_padding(Margin(5));
  fstyle->set_margin(Margin(5));
}
}  // namespace ui
}  // namespace azer

