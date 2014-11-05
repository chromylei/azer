#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/style.h"
#include "base/basictypes.h"

namespace gfx {
class Font;
class FontList;
class Canvas;
}

namespace azer {
namespace ui {

class Canvas;
class StyleGroup;
/**
 * class Theme
 * Theme 帮助统一 UI 风格
 */

class Theme;
class AZER_WIDGET_EXPORT ThemeWidget {
 public:
  virtual void OnThemeChangeing(Theme* orgtheme, Theme* newtheme) = 0;
  virtual void OnThemeChanged(Theme* theme) = 0;
};

class AZER_WIDGET_EXPORT Theme {
 public:
  Theme();
  virtual ~Theme() {}
  virtual bool Init() = 0;

  static Theme* Current();
  static void SetTheme(Theme* theme);

 protected:
  static Theme* theme_;
  DISALLOW_COPY_AND_ASSIGN(Theme);
};

class AZER_EXPORT DefaultTheme : public Theme {
 public:
  DefaultTheme();
  virtual ~DefaultTheme();
  virtual bool Init() OVERRIDE;

  StyleGroup* control_group() { return control_group_.get();}

  uint32 alpha() const { return alpha_;}
 private:
  uint32 alpha_;
  std::unique_ptr<StyleGroup> control_group_;
  DISALLOW_COPY_AND_ASSIGN(DefaultTheme);
};
}  // namespace ui
}  // namespace azer
