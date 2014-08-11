#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/control.h"
#include "base/strings/string16.h"

namespace azer {

class AZER_WIDGET_EXPORT Label : public azer::ControlWidget {
 public:
  void SetText(const base::string16& str);
  void Append(const base::string16& str);

  // in format ARGB
  void SetFontColor(uint32 color) { font_color_ = color;}
  uint32 GetFontColor() const { return font_color_;}
  void SetFillColor(uint32 color) { fill_color_ = color;}
  uint32 GetFillColor() const { return fill_color_;}

  static Label* Create(const gfx::Rect& rect, Window* parent);

  virtual void Redraw(gfx::Canvas* canvas);
  virtual void Render() {}
 protected:
  Label(const gfx::Rect& rc, Window* parent);

  void Redraw();

  bool Init();
  base::string16 text_;
  uint32 font_color_;
  uint32 fill_color_;
  gfx::Canvas* canvas_;
  DISALLOW_COPY_AND_ASSIGN(Label);
};
}  // namespace azer
