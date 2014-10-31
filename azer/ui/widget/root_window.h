#pragma once

#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"

namespace gfx {
class Canvas;
}

namespace azer {
class WindowHost;
class RenderSystem;
namespace ui {

class Context;

class AZER_EXPORT RootWindow : public Widget {
 public:
  RootWindow(Context* context)
      : Widget(context) {
  }

  virtual ~RootWindow() {}

  bool Init();
  virtual void Redraw(bool force) OVERRIDE;
 protected:
  DISALLOW_COPY_AND_ASSIGN(RootWindow);
};
}  // namespace ui
}  // namespace azer
