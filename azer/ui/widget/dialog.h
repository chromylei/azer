#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/widget/widget.h"
#include "base/basictypes.h"

namespace azer {
namespace ui {

class Canvas;
class Context;
class AZER_EXPORT Dialog : public Widget {
 public:
  Dialog(const gfx::Rect& rect, Widget* parent);
 protected:
  DISALLOW_COPY_AND_ASSIGN(Dialog); 
};
}  // namespace ui
}  // namespace azer
