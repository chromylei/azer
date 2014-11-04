#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget_property.h"

namespace azer {
namespace ui {
class AZER_WIDGET_EXPORT WidgetObserver {
 public:
  WidgetObserver();
  virtual ~WidgetObserver() {}

  virtual void OnHide() = 0;
  virtual void OnShow() = 0;
  virtual void OnBoundsChanging(const gfx::Rect& newrect) = 0;
  virtual void OnBoundsChanged(const gfx::Rect& newrect) = 0;
  virtual void OnPropertyChanging(const StringType& name,
                                  PropertyValuePtr& v) = 0;
  virtual void OnPropertyChanged(const StringType& name,
                                 PropertyValuePtr& v) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(WidgetObserver);
};
}  // namespace ui
}  // namespace azer
