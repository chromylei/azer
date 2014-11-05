#include "azer/ui/widget/widget.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/context.h"

namespace azer {
namespace ui {
Widget::Widget(Context* context)
    : context_(context)
    , surface_dirty_(true) {
}

Widget::Widget(const gfx::Rect& rect, Context* context)
    : context_(context)
    , rect_(rect)
    , surface_dirty_(true) {
  canvas_ = new Canvas(this, context->GetCanvas());
}

gfx::Point Widget::ConvertPointToSurface(const gfx::Point& org_pt) {
  gfx::Point pt = org_pt;
  Widget* widget = this;
  while (widget) {
    gfx::Point norg = std::move(widget->origin());
    pt.Offset(norg.x(), norg.y());
    widget = widget->parent();
  }

  return pt;
}

gfx::Rect Widget::ConvertRectToSurface(const gfx::Rect& rc) {
  gfx::Rect r = rc;
  gfx::Point pt = std::move(Widget::ConvertPointToSurface(r.origin()));
  r.set_origin(pt);
  return r;
}

}  // namespace ui
}  // namespace azer
