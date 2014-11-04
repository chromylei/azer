#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget.h"
#include "base/basictypes.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {

class Canvas;
class Context;

class AZER_WIDGET_EXPORT Control : public Widget {
 public:
  Control(const gfx::Rect& rect, Widget* parent);
  virtual ~Control();

  virtual void Redraw(bool force) = 0;

  Widget* parent() { return parent_;}

  virtual void OnHide() OVERRIDE;
  virtual void OnShow() OVERRIDE;
  virtual void OnBoundsChanging(const gfx::Rect& newrect) OVERRIDE;
  virtual void OnBoundsChanged(const gfx::Rect& newrect) OVERRIDE;
 protected:
  Widget* parent_;
  gfx::Rect absolute_rect_;
  DISALLOW_COPY_AND_ASSIGN(Control);
};
}  // namespace ui
}  // namespace azer
