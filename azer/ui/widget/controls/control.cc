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

void Control::OnHide() {
}

void Control::OnShow() {
}

void Control::OnBoundsChanging(const gfx::Rect& r) {
}

void Control::OnBoundsChanged(const gfx::Rect& r) {
}
}  // namespace ui
}  // namespace azer

