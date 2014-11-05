#include "azer/ui/widget/theme/shadow_border.h"
#include "azer/ui/widget/canvas.h"

#include "ui/gfx/shadow_value.h"
#include "ui/gfx/skia_util.h"

namespace azer {
namespace ui {
ShadowBorder::ShadowBorder(int32 blur, uint32 argb, int32 vertical_offset,
                           int32 horizontal_offset)
    : blur_(blur)
    , color_(argb)
    , vertical_offset_(vertical_offset)
    , horizontal_offset_(horizontal_offset) {
}

ShadowBorder::~ShadowBorder() {
}

gfx::Insets ShadowBorder::GetInsets() const {
  return gfx::Insets(blur_ / 2 - vertical_offset_,
                      blur_ / 2 - horizontal_offset_,
                      blur_ / 2 + vertical_offset_,
                      blur_ / 2 + horizontal_offset_);
}

void ShadowBorder::Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) {
  Canvas* canvas = widget->GetCanvas();
  SkPaint paint;
  gfx::ShadowValues shadows;
  shadows.push_back(gfx::ShadowValue(gfx::Point(), blur_, color_));
  skia::RefPtr<SkDrawLooper> looper = gfx::CreateShadowDrawLooper(shadows);
  paint.setLooper(looper.get());
  paint.setColor(SK_ColorTRANSPARENT);
  paint.setStrokeJoin(SkPaint::kRound_Join);
  gfx::Rect bounds(rect.size());
  bounds.Inset(gfx::Insets(blur_ / 2, blur_ / 2, blur_ / 2, blur_ / 2));
  canvas->DrawRect(bounds, paint);
}
}  // namespace ui
}  // namespace azer

