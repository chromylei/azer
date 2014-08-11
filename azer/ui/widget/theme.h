#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"

namespace gfx {
class Font;
class FontList;
class Canvas;
}  // namespace gfx

namespace azer {
class AZER_WIDGET_EXPORT Theme {
 public:
  virtual ~Theme();
  static std::shared_ptr<Theme> GetTheme();
  static void SetCurrentTheme(std::shared_ptr<Theme> theme);

  const gfx::FontList& LabelFont() const { return *label_font_;}
  void SetLabelFont(const gfx::FontList& font);
  int32 LabelFontDrawFlags() const {return label_font_draw_flags_;}
  void SetLabelFontDrawFlags(int32 v) {label_font_draw_flags_ = v;}

public:
  static const uint32 kDefaultPancelFillColor;
  static const uint32 kDefaultLabelFillColor;
private:
  Theme();

  void InitDefaultTheme();

  static std::shared_ptr<Theme> current_theme_;
  gfx::FontList* label_font_;
  int32 label_font_draw_flags_;
  DISALLOW_COPY_AND_ASSIGN(Theme);
};

typedef std::shared_ptr<Theme> ThemePtr;

int32 AZER_WIDGET_EXPORT ClearTextAlignFlags(int32 flag);

}  // namespace azer
