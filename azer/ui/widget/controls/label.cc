#include "azer/ui/widget/controls/label.h"

#include "azer/ui/widget/context.h"
#include "azer/ui/widget/widget.h"
#include "azer/ui/widget/canvas.h"
#include "azer/ui/widget/theme/theme.h"
#include "azer/ui/widget/theme/painter.h"
#include "azer/ui/widget/theme/common_painter.h"

namespace azer {
namespace ui {

const std::string Label::kTextPainterName = "label_text";
const std::string Label::kBorderPainterName = "label_text";

Label::Label(const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent) {
}

Label::Label(const StringType& str, const gfx::Rect& rect, Widget* parent)
    : Control(rect, parent)
    , text_(str) {
}

void Label::Redraw(bool force) {
  DCHECK(border_painter_.get());
  DCHECK(text_painter_.get());
  DCHECK(frame_painter_.get());

  TextPainter::TextContext ctx;
  ctx.SetText(text_);
  Canvas* canvas = canvas_;
  gfx::Rect rect(0, 0, rect_.width(), rect_.height());
  bg_painter_->Paint(this, rect, NULL);
  frame_painter_->Paint(this, rect, NULL);
  text_painter_->Paint(this, rect, &ctx);
  
/*
  const int32 default_label_draw_flags = gfx::Canvas::TEXT_ALIGN_LEFT
      | gfx::Canvas::NO_ELLIPSIS
      | gfx::Canvas::NO_SUBPIXEL_RENDERING;

  gfx::Rect rect(0, 0, rect_.width(), rect_.height());
  
  Theme* theme = Theme::Current();
  const LabelStyle* style = theme->label_style();
  const TextStyle* text_style = style->text_style();
  const FrameStyle* frame_style = style->frame_style();
  Painter painter;
  painter.begin(canvas);
  painter.DrawLabelText(rect, text_, text_style, frame_style);
  painter.DrawFrame(rect, style->frame_style());
  painter.end();
  */
  canvas->end();

}

void Label::OnStyleChangled(StyleGroup* group) {
  border_painter_ = group->GetPainter(Control::kBorderPainterName);
  text_painter_  = group->GetPainter(Control::kTextPainterName);
  frame_painter_ = group->GetPainter(Control::kFramePainterName);
  bg_painter_ = group->GetPainter(Control::kBackgroundPainterName);
}
}  // namespace ui
}  // namespace azer

