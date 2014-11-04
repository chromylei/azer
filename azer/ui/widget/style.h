#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {
namespace ui {

struct Margin {
  int32 left;
  int32 top;
  int32 right;
  int32 bottom;

  Margin() : left(0), top(0), right(0), bottom(0) {}
  Margin(int32 v) : left(v), top(v), right(v), bottom(v) {}
  Margin(int32 v1, int32 v2) : left(v1), top(v2), right(v1), bottom(v2) {}
  Margin(int32 l, int32 t, int32 r, int32 b)
      : left(l), top(t), right(r), bottom(b) {
  }
};

class AZER_WIDGET_EXPORT ShadowStyle {
 public:
  ShadowStyle();
  enum Style {
  };
  int32 width();
 private:
  int32 width_;
  uint32 shadow_color_;
  uint32 light_color_;
  DISALLOW_COPY_AND_ASSIGN(ShadowStyle);
};

class AZER_WIDGET_EXPORT BorderStyle {
 public:
  BorderStyle();

  enum Style {
    kNone = 0,
    kSingleLine = 1,
    kFixed3D,
    kSucken,
  };

  

  int32 width();  // border width
  Style style() const { return style_;}
  uint32 color() const { return color_;}
 private:
  Style style_;
  uint32 width_;
  uint32 color_;
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

  const Margin& margin() { return margin_;}
  const Margin& padding() { return padding_;}
 private:
  Margin padding_;
  Margin margin_;
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
