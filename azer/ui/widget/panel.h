#pragma once

#include "base/basictypes.h"
#include "ui/gfx/geometry/rect.h"
#include "azer/ui/widget/widget.h"

namespace azer {
namespace ui {

class Context;
class AZER_EXPORT Panel : public Widget {
 public:
  Panel(Context* context);
  virtual ~Panel() {}

  virtual void Redraw(bool force) OVERRIDE;
 protected:
 private:
  DISALLOW_COPY_AND_ASSIGN(Panel);
};
}  // namespace ui
}  // namespace azer
