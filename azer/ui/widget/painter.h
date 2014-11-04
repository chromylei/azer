#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/base/string.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {

class Canvas;
class BorderStyle;
class FrameStyle;
class TextStyle;

class Painter {
 public:
  Painter();

  void begin(Canvas* canvas);
  void end();
  virtual void DrawFrame(const gfx::Rect& rect, const FrameStyle* style);
  virtual void DrawLabelText(const gfx::Rect& rect, const StringType& text,
                             const TextStyle* style, const FrameStyle* fstyle);
 private:
  Canvas* canvas_;
  DISALLOW_COPY_AND_ASSIGN(Painter);
};
}  // namespace ui
}  // namespace azer
