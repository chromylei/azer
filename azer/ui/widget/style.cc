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
}  // namespace ui
}  // namespace azer
