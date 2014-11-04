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

gfx::Rect CalcFrameRect(const gfx::Rect& rc, const Margin& margin);
gfx::Rect CalcFrameRect(const gfx::Rect& framerc, const Margin& margin);
gfx::Rect CalcPaddingRect(const gfx::Rect& rc, const Margin& margin,
                          const Margin& padding);

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
  void set_color(uint32 c) { color_ = c;}
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

  void set_bgcolor(uint32 c) { bgcolor_ = c; }
  uint32 bgcolor() const { return bgcolor_;}

  const Margin& margin() const { return margin_;}
  const Margin& padding() const { return padding_;}
  void set_margin(const Margin& m) { margin_ = m; }
  void set_padding(const Margin& p) { padding_ = p; }
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
  void set_color(uint32 c) { color_ = c; }

  uint32 color() const { return color_;}
  const gfx::FontList* font_list() const { return &fontlist_;}
  gfx::FontList* font_list() { return &fontlist_;}
 private:
  uint32 color_;
  gfx::FontList fontlist_;
  DISALLOW_COPY_AND_ASSIGN(TextStyle);
};
}  // namespace ui
}  // namespace azer
