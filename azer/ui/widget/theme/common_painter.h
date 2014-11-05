#pragma once

#include "azer/ui/widget/export.h"
#include "azer/base/string.h"
#include "azer/ui/widget/theme/painter.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/insets.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {
namespace ui {

class Widget;
class AZER_WIDGET_EXPORT ImagePainter : public Painter {
 public:
  ImagePainter() {}
  virtual ~ImagePainter() {}
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(ImagePainter);
};

class AZER_WIDGET_EXPORT BackgroundPainter : public Painter {
 public:
  BackgroundPainter(uint32 bgcolor_argb);
  virtual ~BackgroundPainter();
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) OVERRIDE;
 private:
  uint32 color_;
  DISALLOW_COPY_AND_ASSIGN(BackgroundPainter);
};

class AZER_WIDGET_EXPORT TextPainter : public Painter {
 public:
  class TextContext : public Painter::Context {
   public:
    const StringType& text() const { return text_;}
    void SetText(const StringType& text) { text_ = text;}
   private:
    StringType text_;
  };

  TextPainter(uint32 font_argb, const std::string& fontstr);
  virtual ~TextPainter();
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) OVERRIDE;
 private:
  uint32 color_;
  gfx::FontList fontlist_;
  DISALLOW_COPY_AND_ASSIGN(TextPainter);
};

}  // namespace ui
}  // namespace azer
