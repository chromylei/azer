#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"
#include "ui/gfx/font.h"

namespace azer {
namespace ui {

class AZER_EXPORT ShadowStyle {
 public:
 private:
  uint32 shadow_color_;
};

class AZER_EXPORT BorderStyle {
 public:
  enum Style {
    kFlat = 1,
    kSucken,
  };
  bool has_border();
  Style style() const { return style_;}
 private:
  Style style_;
  ShadowStyle shadow_;
};

class AZER_EXPORT TextStyle {
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

  TextStyle(const StringType& fontname, int32 font_size, uint32 style);
 private:
  uint32 bgcolor_;
  uint32 fgcolor_;
};
}  // namespace ui
}  // namespace azer
