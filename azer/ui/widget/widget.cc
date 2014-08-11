#include "azer/ui/widget/widget.h"

#include "base/logging.h"
#include "ui/gfx/canvas.h"

namespace azer {
Widget::~Widget() {
  Widget* child = first_child();
  while (child) {
    Widget* next = child->next_sibling();
    delete child;
    child = next;
  }
}

gfx::Point Widget::ConvertPointToSurface(const gfx::Point& org_pt) {
  gfx::Point pt = org_pt;
  Widget* widget = this;
  while (widget && !widget->HasSurface()) {
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

void Widget::Sort() {
}
}  // namespace azer
