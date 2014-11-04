#pragma once

#include <set>
#include "base/basictypes.h"
#include "base/lazy_instance.h"

namespace azer {
namespace ui {

class WidgetManager {
 public:
  int64 allocate_id();
  void free_id(int64 widget);
  static WidgetManager* GetInstance();
 private:
  WidgetManager();
  int64 allocated_;

  std::set<int64> released_;
  friend struct ::base::DefaultLazyInstanceTraits<WidgetManager>;
  DISALLOW_COPY_AND_ASSIGN(WidgetManager);
};
}  // namespace ui
}  // namespace azer
