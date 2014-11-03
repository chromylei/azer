#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/widget/widget.h"
#include "azer/base/string.h"
#include "base/basictypes.h"

#include "ui/gfx/canvas.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/point.h"


namespace azer {
namespace ui {

class Widget;

/**
 * Canvas
 * Canvas help draw control on local coordinate system, but not
 * surface coordinate system.
 */
class AZER_EXPORT Canvas {
 public:
  Canvas(Widget* widget, gfx::Canvas* canvas);

  void reset(const gfx::Rect& rect);
  void begin();
  void end();

  // Fills the entire canvas' bitmap (restricted to current clip) with
  // specified |color| using a transfer mode of SkXfermode::kSrcOver_Mode.
  void DrawColor(SkColor color);

  // Fills the entire canvas' bitmap (restricted to current clip) with
  // specified |color| and |mode|.
  void DrawColor(SkColor color, SkXfermode::Mode mode);

  // Fills |rect| with |color| using a transfer mode of
  // SkXfermode::kSrcOver_Mode.
  void FillRect(const gfx::Rect& rect, SkColor color);

  // Fills |rect| with the specified |color| and |mode|.
  void FillRect(const gfx::Rect& rect, SkColor color, SkXfermode::Mode mode);

  // Draws a single pixel rect in the specified region with the specified
  // color, using a transfer mode of SkXfermode::kSrcOver_Mode.
  //
  // NOTE: if you need a single pixel line, use DrawLine.
  void DrawRect(const gfx::Rect& rect, SkColor color);

  // Draws a single pixel rect in the specified region with the specified
  // color and transfer mode.
  //
  // NOTE: if you need a single pixel line, use DrawLine.
  void DrawRect(const gfx::Rect& rect, SkColor color, SkXfermode::Mode mode);

  // Draws the given rectangle with the given |paint| parameters.
  void DrawRect(const gfx::Rect& rect, const SkPaint& paint);

  // Draw the given point with the given |paint| parameters.
  void DrawPoint(const gfx::Point& p, const SkPaint& paint);

  // Draws a single pixel line with the specified color.
  void DrawLine(const gfx::Point& p1, const gfx::Point& p2, SkColor color);

  // Draws a line with the given |paint| parameters.
  void DrawLine(const gfx::Point& p1, const gfx::Point& p2, const SkPaint& paint);

  // Draws a circle with the given |paint| parameters.
  void DrawCircle(const gfx::Point& center_point,
                  int radius,
                  const SkPaint& paint);

  // Draws the given rectangle with rounded corners of |radius| using the
  // given |paint| parameters.
  void DrawRoundRect(const gfx::Rect& rect, int radius, const SkPaint& paint);

  // Draws text with the specified color, fonts and location. The last argument
  // specifies flags for how the text should be rendered. It can be one of
  // TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT or TEXT_ALIGN_LEFT.
  void DrawStringRectWithFlags(const base::string16& text,
                               const gfx::FontList& font_list,
                               SkColor color,
                               const gfx::Rect& display_rect,
                               int flags);
 private:
  Widget* widget_;
  gfx::Canvas* canvas_;
  gfx::Rect TransRect(const gfx::Rect& r);
  gfx::Point TransPoint(const gfx::Point& pt);
  DISALLOW_COPY_AND_ASSIGN(Canvas);
};

inline void Canvas::DrawColor(SkColor color) {
  canvas_->DrawColor(color);
}

inline void Canvas::DrawColor(SkColor color, SkXfermode::Mode mode) {
  canvas_->DrawColor(color, mode);
}

inline void Canvas::FillRect(const gfx::Rect& rect, SkColor color) {
  canvas_->FillRect(TransRect(rect), color);
}

inline void Canvas::FillRect(const gfx::Rect& rect, SkColor color,
                               SkXfermode::Mode mode) {
  canvas_->FillRect(TransRect(rect), color, mode);
}

inline void Canvas::DrawRect(const gfx::Rect& rect, SkColor color) {
  canvas_->DrawRect(TransRect(rect), color);
}

inline void Canvas::DrawRect(const gfx::Rect& rect, SkColor color,
                               SkXfermode::Mode mode) {
  canvas_->DrawRect(TransRect(rect), color, mode);
}

inline void Canvas::DrawRect(const gfx::Rect& rect, const SkPaint& paint) {
  canvas_->DrawRect(TransRect(rect), paint);
}

inline void Canvas::DrawPoint(const gfx::Point& p, const SkPaint& paint) {
  canvas_->DrawPoint(TransPoint(p), paint);
}

inline void Canvas::DrawLine(const gfx::Point& p1, const gfx::Point& p2,
                               SkColor color) {
  canvas_->DrawLine(TransPoint(p1), TransPoint(p2), color);
}

inline void Canvas::DrawLine(const gfx::Point& p1, const gfx::Point& p2,
                               const SkPaint& paint) {
  canvas_->DrawLine(TransPoint(p1), TransPoint(p2), paint);
}

inline void Canvas::DrawCircle(const gfx::Point& center_point,
                                 int radius,
                                 const SkPaint& paint) {
  canvas_->DrawCircle(TransPoint(center_point), radius, paint);
}

inline void Canvas::DrawRoundRect(const gfx::Rect& rect, int radius,
                                    const SkPaint& paint) {
  canvas_->DrawRoundRect(TransRect(rect), radius, paint);
}

inline void Canvas::DrawStringRectWithFlags(const base::string16& text,
                                            const gfx::FontList& font_list,
                                            SkColor color,
                                            const gfx::Rect& display_rect,
                                            int flags) {
  canvas_->DrawStringRectWithFlags(text, font_list, color,
                                   TransRect(display_rect), flags);
}

inline void Canvas::begin() {}
inline void Canvas::end() {
  canvas_->Save();
  canvas_->Restore();
}

inline gfx::Rect Canvas::TransRect(const gfx::Rect& r) {
  return widget_->ConvertRectToSurface(r);
}

inline gfx::Point Canvas::TransPoint(const gfx::Point& pt) {
  return widget_->ConvertPointToSurface(pt);
}
}  // namespace ui
}  // namespace azer
