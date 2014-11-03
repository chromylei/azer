#pragma once

#include "azer/base/render_export.h"
#include "base/basictypes.h"

namespace gfx {
class Font;
class FontList;
class Canvas;
}

namespace azer {
namespace ui {

class Canvas;

/**
 * class Theme
 * Theme 帮助统一 UI 风格
 */

class AZER_EXPORT Theme {
 public:
  Theme();
  virtual ~Theme() {}
  virtual bool Init() = 0;

  static Theme* Current();
  static void SetTheme(Theme* theme);

  const gfx::FontList& label_font() const { return *label_font_;}
 protected:
  gfx::FontList* label_font_;
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
