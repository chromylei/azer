#include "azer/ui/widget/theme/border.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/widget.h"

namespace azer {
namespace ui {

SingeLineBorderPainter::SingeLineBorderPainter(int32 width, uint32 color)
    : width_(width)
    , color_(color) {
}


void SingeLineBorderPainter::Paint(Widget* w, const gfx::Rect& r,  Context* ctx) {
  Canvas* canvas = w->GetCanvas();
  canvas->DrawLine(r.origin(), r.top_right(), color_);
  canvas->DrawLine(r.origin(), r.bottom_left(), color_);
  canvas->DrawLine(r.bottom_right(), r.top_right(), color_);
  canvas->DrawLine(r.bottom_right(), r.bottom_left(), color_);
}

}  // namespace ui
}  // namespace azer
