#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "ui/gfx/geometry/size.h"

namespace azer {
namespace ui {

class Widget;

class AZER_WIDGET_EXPORT LayoutManager {
 public:
  virtual ~LayoutManager();
  virtual void Layout(Widget* host) = 0;

  virtual gfx::Size CalcPreferredSize() const = 0;

  virtual void Add(Widget* host, Widget* w) = 0;
  virtual void Remove(Widget* host, Widget* w) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(LayoutManager);
};
} // namespace ui
} // namespace azer
