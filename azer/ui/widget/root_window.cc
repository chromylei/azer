#include "azer/ui/widget/root_window.h"
#include "azer/ui/widget/context.h"

#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkTemplates.h"
#include "SkString.h"

namespace azer {
namespace ui {

const float RootWindow::kMinDrawInterval = 0.00;
void RootWindow::Redraw(bool force) {
  ::base::TimeTicks now = ::base::TimeTicks::HighResNow();
  ::base::TimeDelta delta = now - prev_draw_;
  if (delta.InSecondsF() > kMinDrawInterval) {
    Widget* widget = last_child();
    while (widget) {
      widget->Redraw(force);
      widget = widget->prev_sibling();
    }

    SkCanvas* canvas = context_->GetCanvas()->GetSkCanvas();
    canvas->clear(SkColorSetARGB(0,0,0,0));
    SkString text("Hello, Skia World");
    SkPaint paint;
    paint.setARGB(255, 0, 0, 0);
    paint.setAntiAlias(true);
    paint.setTextSize(SkIntToScalar(30));
    SkScalar width = 800;
    SkScalar height = 600;
    canvas->drawARGB(255, 255, 255, 255);
    SkScalar x = 80, y = 60;
    canvas->drawText(text.c_str(), text.size(), x, y, paint);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(10);

    SkRect rect;
    paint.setARGB(255, 0, 0, 0);
    rect.set(50, 100, 200, 200);
    canvas->drawRoundRect(rect, 20, 20, paint);

    canvas->drawLine(10, 300, 300, 300, paint);
    canvas->drawCircle(100, 400, 50, paint);
    prev_draw_ = ::base::TimeTicks::HighResNow();
    canvas->flush();
  }
}

bool RootWindow::Init() {
  Redraw(true);
  return true;
}
}  // namespace ui
}  // namespace azer
