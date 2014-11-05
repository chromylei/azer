#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/views/painter.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/insets.h"

namespace azer {
namespace ui {

class Widget;
class ShadowBorder;
class AZER_WIDGET_EXPORT PainterManager {
 public:
  PainterManager() {}
  virtual ~PainterManager() {}

  virtual ShadowBorder* CreateShadowPainter() = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(PainterManager);
};

}  // namespace ui
}  // namespace azer
