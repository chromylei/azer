#pragma once



#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget.h"

namespace azer {

class Window;

class AZER_WIDGET_EXPORT ControlWidget : public azer::Widget {
 public:
 protected:
  ControlWidget(const gfx::Rect& rc, Window* parent)
      : Widget(rc) {
    DCHECK(parent != NULL);
    ((Widget*)parent)->AddChild(this);
  }

  DISALLOW_COPY_AND_ASSIGN(ControlWidget);
};
}  // namespace azer
