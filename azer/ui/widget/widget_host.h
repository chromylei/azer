#pragma once

#pragma once

#include "base/basictypes.h"
#include "base/time/time.h"
#include "azer/ui/widget/widget.h"

namespace gfx {
class Canvas;
}

namespace azer {
class WindowHost;
class RenderSystem;
namespace ui {

class Context;

class AZER_EXPORT WidgetHost : public Widget {
 public:
  WidgetHost(Context* context)
      : Widget(context) {
  }

  virtual ~WidgetHost() {}

  bool Init();
  virtual void Redraw(bool force) OVERRIDE;

  virtual void OnHide() OVERRIDE;
  virtual void OnShow() OVERRIDE;
  virtual void OnBoundsChanging() OVERRIDE;
  virtual void OnBoundsChanged() OVERRIDE;
 protected:
  const static float kMinDrawInterval;
  base::TimeTicks prev_draw_;
  DISALLOW_COPY_AND_ASSIGN(WidgetHost);
};
}  // namespace ui
}  // namespace azer
