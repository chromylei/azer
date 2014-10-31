#include "azer/ui/widget/panel.h"
#include "azer/ui/widget/context.h"

#include "azer/render/skia/canvas.h"

namespace azer {
namespace ui {
Panel::Panel(Context* context)
    : Widget(context) {
}

void Panel::Redraw(bool force) {
}
}  // namespace ui {
}  // namespace azer
