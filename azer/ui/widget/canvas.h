#pragma once

#include "azer/render/render_export.h"
#include "azer/ui/widget/window.h"
#include "base/basictypes.h"

namespace azer {
namespace ui {
/**
 * Canvas
 * 在 ui 框架下，所有的 control 实际上最终结果显示在一个 texture 上
 * 但针对于每一个 control 他们的绘制坐标系又是以自己的坐标系为参考的
 * 此类的功能就是提供一个以本窗口坐标系的 canvas 对象
 *
 */
class Canvas {
 public:
  Canvas();
 private:
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};
}  // namespace ui
}  // namespace azer
