#include "azer/ui/widget/widget_host.h"
#include "azer/ui/widget/context.h"

#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkTemplates.h"
#include "SkString.h"

#include "ui/gfx/canvas.h"

namespace azer {
namespace ui {

const float WidgetHost::kMinDrawInterval = 0.00;
void WidgetHost::Redraw(bool force) {
  ::base::TimeTicks now = ::base::TimeTicks::HighResNow();
  ::base::TimeDelta delta = now - prev_draw_;
  if (delta.InSecondsF() > kMinDrawInterval) {
    SkCanvas* canvas = context_->GetSkCanvas()->GetSkCanvas();
    canvas->clear(SkColorSetARGB(0,0,0,0));
    /*
    SkString text("Hello, Skia World");
    SkPaint paint;
    paint.setARGB(255, 30, 30, 30);
    paint.setAntiAlias(true);
    paint.setTextSize(SkIntToScalar(30));
    SkScalar width = 800;
    SkScalar height = 600;
    canvas->drawARGB(0, 255, 255, 255);
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
    */
    Widget* widget = last_child();
    while (widget) {
      widget->Redraw(force);
      widget = widget->prev_sibling();
    }
    canvas->flush();
    prev_draw_ = ::base::TimeTicks::HighResNow();
  }
}

bool WidgetHost::Init() {
  Redraw(true);
  return true;
}

void WidgetHost::OnHide() {
}

void WidgetHost::OnShow() {
}

void WidgetHost::OnBoundsChanging(const gfx::Rect& r) {
}

void WidgetHost::OnBoundsChanged(const gfx::Rect& r) {
}
}  // namespace ui
}  // namespace azer
