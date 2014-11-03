#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/widget/widget.h"
#include "base/basictypes.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {

class Canvas;
class Context;

class AZER_EXPORT Control : public Widget {
 public:
  Control(const gfx::Rect& rect, Widget* parent);
  virtual ~Control();

  virtual void Hide() OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void Redraw(bool force) = 0;

  Widget* parent() { return parent_;}
 private:
  void CalcAbsoluteRect();

  Canvas* canvas_;
  Widget* parent_;
  gfx::Rect absolute_rect_;
  DISALLOW_COPY_AND_ASSIGN(Control);
};
}  // namespace ui
}  // namespace azer
