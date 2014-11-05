#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/border.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT ShadowBorder : public BorderPainter {
 public:
  ShadowBorder(int32 blur, uint32 argb, int32 vertical_offset,
               int32 horizontal_offset);
  ~ShadowBorder();
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) OVERRIDE;
  gfx::Insets GetInsets() const;
 private:
  // blur amount of pixel
  const int32 blur_;
  const uint32 color_;
  const int32 vertical_offset_;
  const int32 horizontal_offset_;
  DISALLOW_COPY_AND_ASSIGN(ShadowBorder);
};
}  // namespace ui
}  // namespace azer
