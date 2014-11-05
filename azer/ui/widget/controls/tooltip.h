#pragma once

#include "base/basictypes.h"
#include "azer/base/string.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/views/style.h"
#include "azer/ui/widget/controls/control.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT TooltipStyle {
 public:
  TooltipStyle() {}

  FrameStyle* frame_style() { return &frame_style_;}
  const FrameStyle* frame_style() const { return &frame_style_;}
 private:
  FrameStyle frame_style_;
  DISALLOW_COPY_AND_ASSIGN(TooltipStyle);
};

class AZER_WIDGET_EXPORT Tooltip : public Control {
 public:
  Tooltip(const gfx::Rect& rect, Widget* parent);
  virtual ~Tooltip();

  void SetText(const StringType& text);
  virtual void Redraw(bool force) OVERRIDE;
 private:
  StringType text_;
  DISALLOW_COPY_AND_ASSIGN(Tooltip);
};
}  // namespace ui
}  // namespace azer
