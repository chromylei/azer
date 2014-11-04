#include "azer/ui/widget/controls/control.h"
#include "azer/ui/widget/canvas.h"
#include "base/logging.h"

namespace azer {
namespace ui {
Control::Control(const gfx::Rect& rect, Widget* parent)
    : Widget(rect, parent->GetContext())
    , parent_(parent) {
  DCHECK(parent != NULL);
  parent->AddChild(this);
}

Control::~Control() {
  if (canvas_) {
    delete canvas_;
  }
}

void Control::Hide() {
}

void Control::Show() {
}

}  // namespace ui
}  // namespace azer

