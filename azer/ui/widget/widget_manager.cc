#include "azer/ui/widget/widget_manager.h"
#include "base/lazy_instance.h"
#include "base/logging.h"

namespace azer {
namespace ui {

base::LazyInstance<WidgetManager>::Leaky widget_manager_;

WidgetManager* WidgetManager::GetInstance() {
  return widget_manager_.Pointer();
}

WidgetManager::WidgetManager()
    : allocated_(0) {
}

int64 WidgetManager::allocate_id() {
  if (released_.size() > 0) {
    int64 v = *released_.begin();
    released_.erase(released_.begin());
    return v;
  } else {
    return ++allocated_;
  }
}

void WidgetManager::free_id(int64 widget) {
  DCHECK(released_.find(widget) != released_.end());
  released_.insert(widget);
}
}  // namespace ui
}  // namespace azer
