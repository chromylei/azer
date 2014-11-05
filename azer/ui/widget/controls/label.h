#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/controls/control.h"
#include "azer/ui/widget/views/style.h"
#include "base/basictypes.h"
#include "azer/base/string.h"

namespace azer {
namespace ui {
class Context;

class AZER_WIDGET_EXPORT LabelStyle {
 public:
  LabelStyle() {}

  FrameStyle* frame_style() { return &frame_style_;}
  const FrameStyle* frame_style() const { return &frame_style_;}

  TextStyle* text_style() { return &text_style_;}
  const TextStyle* text_style() const { return &text_style_;}
 private:
  FrameStyle frame_style_;
  TextStyle text_style_;
  DISALLOW_COPY_AND_ASSIGN(LabelStyle);
};

class AZER_WIDGET_EXPORT Label : public Control {
 public:
  Label(const gfx::Rect& rect, Widget* parent);
  Label(const StringType& str, const gfx::Rect& rect, Widget* parent);

  void SetText(const StringType& str) {text_ = str;}
  const StringType text() const { return text_;}
 public:
  virtual void Redraw(bool force) OVERRIDE;
 protected:
  StringType text_;
  DISALLOW_COPY_AND_ASSIGN(Label);
};
}  // namespace ui
}  // namespace azer
