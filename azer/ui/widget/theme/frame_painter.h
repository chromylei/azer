#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/painter.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT FramePainter : public Painter {
 public:
  FramePainter() {}
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(FramePainter);
};

class AZER_WIDGET_EXPORT SingleLineFramePainter : public Painter {
 public:
  SingleLineFramePainter(int32 width, uint32 color);
  ~SingleLineFramePainter();
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) OVERRIDE;
 private:
  uint32 width_;
  uint32 color_;
  DISALLOW_COPY_AND_ASSIGN(SingleLineFramePainter);
};
}  // namespace ui
}  // namespace azer
