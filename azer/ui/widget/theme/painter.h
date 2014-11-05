#pragma once

#include <memory>
#include <map>

#include "azer/ui/widget/export.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/insets.h"

namespace azer {
namespace ui {

class Widget;
class AZER_WIDGET_EXPORT Painter {
 public:
  class Context {
   public:
  };
  Painter() {}
  virtual ~Painter() {}
  virtual void Paint(Widget* widget, const gfx::Rect& rect, Context* ctx) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(Painter);
};

typedef std::shared_ptr<Painter> PainterPtr;
typedef std::map<std::string, PainterPtr> PainterDict;
}  // namespace ui
}  // namespace azer
