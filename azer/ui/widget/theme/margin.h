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
}  // namespace ui
}  // namespace azer
