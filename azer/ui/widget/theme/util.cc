#include "azer/ui/widget/theme/util.h"
#include "azer/ui/widget/theme/style_group.h"
#include "azer/ui/widget/theme/border.h"
#include "azer/ui/widget/theme/frame_painter.h"
#include "azer/ui/widget/theme/common_painter.h"
#include "azer/ui/widget/theme/shadow_border.h"

#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/controls/control.h"
#include "azer/ui/widget/controls/label.h"

namespace azer {
namespace ui {

void InitDefaultControlStyleGroup(StyleGroup* group) {
  std::string textstr = "Arial, Bold 14px";
  uint32 text_color = (uint32)SkColorSetARGBInline(255, 238, 238, 238);
  uint32 border_color = (uint32)SkColorSetARGBInline(255, 0xb, 0xb, 0xb);
  uint32 bg_color = (uint32)SkColorSetARGBInline(128, 64, 64, 64);
  PainterPtr control_border(new SingeLineBorderPainter(1, border_color));
  PainterPtr control_frame(new SingleLineFramePainter(1, border_color));
  PainterPtr control_bg(new BackgroundPainter(bg_color));
  PainterPtr control_text(new TextPainter(text_color, textstr));
  group->SetPainter(Control::kBorderPainterName, control_border);
  group->SetPainter(Control::kFramePainterName, control_frame);
  group->SetPainter(Control::kBackgroundPainterName, control_bg);
  group->SetPainter(Control::kTextPainterName, control_text);
}
}  // namespace ui
}  // namespace azer
