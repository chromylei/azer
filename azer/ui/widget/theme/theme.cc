#include "azer/ui/widget/theme/theme.h"

#include "base/logging.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"
#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/theme/style_group.h"
#include "azer/ui/widget/theme/util.h"

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
  DCHECK(control_group_.get() == NULL);
  control_group_.reset(new StyleGroup());
  InitDefaultControlStyleGroup(control_group_.get());
  return true;
}

}  // namespace ui
}  // namespace azer

