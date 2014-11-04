#pragma once

#include <set>
#include "base/basictypes.h"
#include "azer/base/render_export.h"

namespace azer {
namespace ui {
class AZER_EXPORT WidgetManager {
 public:
  int64 allocate_id();
  void free_id(int64 widget);
  static WidgetManager* GetInstance();
 private:
  WidgetManager();
  int64 allocated_;
  DISALLOW_COPY_AND_ASSIGN(WidgetManager);
};
}  // namespace ui
}  // namespace azer
