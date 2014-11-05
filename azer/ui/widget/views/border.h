#pragma once

#include "azer/ui/widget/export.h"
#include "ui/gfx/insets.h"

namespace azer {
namespace ui {

class Widget;
class AZER_WIDGET_EXPORT Border {
 public:
  Border() {}
  virtual void Paint(Widget* widget) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(Border);
};

}  // namespace ui
}  // namespace azer
