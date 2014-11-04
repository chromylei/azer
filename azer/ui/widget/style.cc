#include "azer/ui/widget/style.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/theme.h"

namespace azer {
namespace ui {
ShadowStyle::ShadowStyle() {
}

BorderStyle::BorderStyle()
    : style_(kSingleLine)
    , width_(1)
    , color_((uint32)SkColorSetARGBInline(255, 192, 192, 192)) {
}

FrameStyle::FrameStyle()
    : bgcolor_((uint32)SkColorSetARGBInline(0, 0, 0, 0)) {
}

TextStyle::TextStyle()
    : bgcolor_((uint32)SkColorSetARGBInline(0, 0, 0, 0))
    , fgcolor_((uint32)SkColorSetARGBInline(255, 192, 192, 192))
    , fontlist_(gfx::FontList("Arial, Bold 14px")) {
}

void TextStyle::SetFont(const std::string& fontstr) {
  fontlist_ = gfx::FontList(fontstr);
}

gfx::Rect CalcFrameRect(const gfx::Rect& rect, const Margin& margin) {
  return gfx::Rect(rect.x() + margin.left,
                   rect.y() + margin.top,
                   rect.width() - margin.left - margin.right,
                   rect.height() - margin.top - margin.bottom);
}

gfx::Rect CalcPaddingRect(const gfx::Rect& rect, const Margin& margin,
                          const Margin& padding) {
  int top = margin.top + padding.top;
  int left = margin.left + padding.left;
  int right = margin.right + padding.right;
  int bottom = margin.bottom + padding.bottom;
  return gfx::Rect(rect.x() + left, rect.y() + top,
                   rect.width() - left - right,
                   rect.height() - top - bottom);
}
}  // namespace ui
}  // namespace azer
