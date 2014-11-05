#pragma once

#include "base/basictypes.h"
#include "azer/base/string.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/painter.h"
#include "azer/ui/widget/controls/control.h"

namespace azer {
namespace ui {

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
