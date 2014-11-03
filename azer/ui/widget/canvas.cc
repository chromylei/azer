#include "azer/ui/widget/canvas.h"

namespace azer {
namespace ui {
Canvas::Canvas(Widget* widget, gfx::Canvas* canvas)
    : canvas_(canvas)
    , widget_(widget) {
}
}  // namespace ui
}  // namespace azer

