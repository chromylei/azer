#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/style.h"
#include "azer/ui/widget/controls/label.h"
#include "base/basictypes.h"

namespace gfx {
class Font;
class FontList;
class Canvas;
}

namespace azer {
namespace ui {

class Canvas;
class LabelStyle;
/**
 * class Theme
 * Theme 帮助统一 UI 风格
 */

class AZER_WIDGET_EXPORT Theme {
 public:
  Theme();
  virtual ~Theme() {}
  virtual bool Init() = 0;

  static Theme* Current();
  static void SetTheme(Theme* theme);

  const LabelStyle* label_style() const { return &label_style_;}
  LabelStyle* label_style() { return &label_style_;}

  virtual void DrawFrame(const gfx::Rect& rect, const BorderStyle* style) = 0;
 protected:
  LabelStyle label_style_;
  static Theme* theme_;
  DISALLOW_COPY_AND_ASSIGN(Theme);
};

class AZER_EXPORT DefaultTheme : public Theme {
 public:
  DefaultTheme() {}
  virtual ~DefaultTheme();
  virtual bool Init() OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(DefaultTheme);
};
}  // namespace ui
}  // namespace azer
