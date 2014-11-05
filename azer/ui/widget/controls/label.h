#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/controls/control.h"
#include "azer/ui/widget/theme/painter.h"
#include "azer/ui/widget/theme/margin.h"
#include "base/basictypes.h"
#include "azer/base/string.h"

namespace azer {
namespace ui {
class Painter;
class Context;

class AZER_WIDGET_EXPORT Label : public Control {
 public:
  Label(const gfx::Rect& rect, Widget* parent);
  Label(const StringType& str, const gfx::Rect& rect, Widget* parent);

  void SetPadding(const Margin& padding) { padding_ = padding;}
  void SetText(const StringType& str) {text_ = str;}
  const StringType text() const { return text_;}
 public:
  virtual void Redraw(bool force) OVERRIDE;
  virtual void OnStyleChangled(StyleGroup* group) OVERRIDE;

  static const std::string kTextPainterName;
  static const std::string kBorderPainterName;
 protected:
  PainterPtr border_painter_;
  PainterPtr text_painter_;
  PainterPtr frame_painter_;
  PainterPtr bg_painter_;
  StringType text_;
  Margin padding_;
  DISALLOW_COPY_AND_ASSIGN(Label);
};
}  // namespace ui
}  // namespace azer
