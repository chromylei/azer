#pragma once

#include <vector>
#include <memory>
#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"

namespace azer {
namespace ui {

class AZER_EXPORT Layout {
 public:
  Layout(Widget* parent);
  virtual ~Layout() {}

  virtual void layout() = 0;
 protected:
  typedef std::vector<WidgetPtr> WidgetVec;
  WidgetVec widgets_;
  DISALLOW_COPY_AND_ASSIGN(Layout);
};
}  // namespace ui
}  // namespace azer
