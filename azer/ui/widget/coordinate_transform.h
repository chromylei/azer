#pragma once

#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {

inline gfx::RectF WindowToRenderer(const gfx::Rect& rc, const gfx::Rect& win) {
  /**
   * 渲染坐标系是以平面中心为原点，与数学坐标系相同的坐标系
   * 窗口坐标系以左上角为原点
   */
  float x = (float)rc.x() / (float)win.width();
  float y = 1 - (float)rc.bottom() / (float)win.height();
  
  return gfx::RectF(x * 2 - 1,
                    y * 2 - 1,
                    2.0f * (float)rc.width() / (float)win.width(),
                    2.0f * (float)rc.height() / (float)win.height());
}
}  // namespace azer
