#include "azer/ui/widget/controls/tooltip.h"

namespace azer {
namespace ui {
Tooltip::Tooltip(const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent) {
}

Tooltip::~Tooltip() {
}

void Tooltip::Redraw(bool force) {
}

void Tooltip::SetText(const StringType& text) {
}
}  // namespace ui
}  // namespace azer
