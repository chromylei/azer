#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/painter.h"
#include "ui/gfx/insets.h"

namespace azer {
namespace ui {

class Widget;
class AZER_WIDGET_EXPORT BorderPainter : public Painter {
 public:
  BorderPainter() {}
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(BorderPainter);
};

class AZER_WIDGET_EXPORT NoneBorderPainter : public Painter {
 public:
  NoneBorderPainter() {}
  virtual void Paint(Widget* w, const gfx::Rect& r,  Context* ctx) OVERRIDE {};
 private:
  DISALLOW_COPY_AND_ASSIGN(NoneBorderPainter);
};

class AZER_WIDGET_EXPORT SingeLineBorderPainter : public Painter {
 public:
  SingeLineBorderPainter(int32 width, uint32 color);
  virtual void Paint(Widget* w, const gfx::Rect& r,  Context* ctx) OVERRIDE;
 private:
  int32 width_;
  uint32 color_;
  DISALLOW_COPY_AND_ASSIGN(SingeLineBorderPainter);
};

}  // namespace ui
}  // namespace azer
