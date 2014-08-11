#include "azer/ui/widget/theme.h"

#include "ui/gfx/canvas.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"
#include "third_party/skia/include/core/SkColor.h"

namespace azer {
const uint32 Theme::kDefaultLabelFillColor = (uint32)SkColorSetARGBInline(0, 0, 0, 0);
const uint32 Theme::kDefaultPancelFillColor = (uint32)SkColorSetARGBInline(64, 32, 32, 32);;

Theme::Theme() {
  label_font_ = new gfx::FontList("Courier New, 14px");
}

Theme::~Theme() {
  if (label_font_) delete label_font_;
}

void Theme::InitDefaultTheme() {
  const int32 default_label_draw_flags = gfx::Canvas::TEXT_ALIGN_LEFT
      | gfx::Canvas::NO_ELLIPSIS
      | gfx::Canvas::NO_SUBPIXEL_RENDERING;
  SetLabelFontDrawFlags(default_label_draw_flags);
}

void Theme::SetLabelFont(const gfx::FontList& font) {
  *label_font_ = font;
}


ThemePtr Theme::current_theme_;
ThemePtr Theme::GetTheme() {
  if (current_theme_.get() == NULL) {
    current_theme_.reset(new Theme);
  }

  return current_theme_;
}

void Theme::SetCurrentTheme(ThemePtr theme) {
  current_theme_ = theme;
}

int32 AZER_WIDGET_EXPORT ClearTextAlignFlags(int32 flag) {
  return flag & ~(gfx::Canvas::TEXT_ALIGN_LEFT |
                  gfx::Canvas::TEXT_ALIGN_CENTER |
                  gfx::Canvas::TEXT_ALIGN_RIGHT);
}
}  // namespace azer

