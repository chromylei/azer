#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"

namespace azer {
namespace ui {

class Canvas;
class BorderStyle;
class TextStyle;

class Painter {
 public:
  Painter();

  void begin(Canvas* canvas);
  void end();
  virtual void DrawFrame(const gfx::Rect& rect, const BorderStyle* style);
  virtual void DrawText(const gfx::Rect& rect, const StringType& text,
                        TextStyle* style);
 private:
  Canvas* canvas_;
  DISALLOW_COPY_AND_ASSIGN(Painter);
};
}  // namespace ui
}  // namespace azer
