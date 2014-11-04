#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT ShadowStyle {
 public:
 private:
  uint32 shadow_color_;
};

class AZER_WIDGET_EXPORT BorderStyle {
 public:
  BorderStyle();

  enum Style {
    kFlat = 1,
    kSucken,
  };
  bool has_border();
  Style style() const { return style_;}
 private:
  Style style_;
  ShadowStyle shadow_;
  DISALLOW_COPY_AND_ASSIGN(BorderStyle);
};

class AZER_WIDGET_EXPORT FrameStyle {
 public:
  FrameStyle();
  const BorderStyle* border_style() const { return &border_style_;}
  BorderStyle* border_style() { return &border_style_;}

  void SetBgcolor(uint32 c) { bgcolor_ = c; }
  uint32 bgcolor() const { return bgcolor_;}
 private:
  BorderStyle border_style_;
  uint32 bgcolor_;
  DISALLOW_COPY_AND_ASSIGN(FrameStyle);
};

class AZER_WIDGET_EXPORT TextStyle {
 public:
  enum Align {
    kLeftAlign,
    kRightAlign,
    kCenterAlign,
  };

  enum Direction {
    kLeftToRight,
    kRightToLeft,
  };

  enum {
    kBold,
  };

  TextStyle();

  void SetFont(const std::string& fontstr);
  void SetBgcolor(uint32 c) { bgcolor_ = c; }
  void SetFgcolor(uint32 c) { fgcolor_ = c; }

  uint32 bgcolor() const { return bgcolor_;}
  uint32 fgcolor() const { return fgcolor_;}
  const gfx::FontList* font_list() const { return &fontlist_;}
  gfx::FontList* font_list() { return &fontlist_;}
 private:
  uint32 bgcolor_;
  uint32 fgcolor_;
  gfx::FontList fontlist_;
  DISALLOW_COPY_AND_ASSIGN(TextStyle);
};
}  // namespace ui
}  // namespace azer
