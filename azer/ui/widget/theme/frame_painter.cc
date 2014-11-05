#include "azer/ui/widget/theme/frame_painter.h"
#include "azer/ui/widget/canvas.h"

namespace azer {
namespace ui {
SingleLineFramePainter::SingleLineFramePainter(int32 width, uint32 color)
    : width_(width)
    , color_(color) {
}

SingleLineFramePainter::~SingleLineFramePainter() {
}

void SingleLineFramePainter::Paint(Widget* widget, const gfx::Rect& rect,
                                   Context* ctx) {
  Canvas* canvas = widget->GetCanvas();
  canvas->DrawLine(rect.origin(), rect.top_right(),  color_);
  canvas->DrawLine(rect.origin(), rect.bottom_left(), color_);
  canvas->DrawLine(rect.bottom_right(), rect.top_right(), color_);
  canvas->DrawLine(rect.bottom_right(), rect.bottom_left(), color_);
}
}  // namespace ui
}  // namespace azer
