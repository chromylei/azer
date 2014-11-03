#pragma once

#include "base/basictypes.h"
#include "ui/gfx/geometry/rect.h"
#include "azer/ui/widget/control.h"

namespace azer {
namespace ui {

class Context;
class AZER_EXPORT Panel : public Control {
 public:
  Panel(const gfx::Rect& rect, Widget* parent);
  virtual ~Panel() {}

  virtual void Redraw(bool force) OVERRIDE;
 protected:
 private:
  DISALLOW_COPY_AND_ASSIGN(Panel);
};
}  // namespace ui
}  // namespace azer
