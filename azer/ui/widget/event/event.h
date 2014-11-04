#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/widget/widget.h"

namespace azer {
namespace ui {
class AZER_EXPORT Event {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(Event);
};

typedef std::shared_ptr<Event> EventPtr;

/**
 * 消息传递的次序
 * 消息从 rootwindow 收取并按照层次关系下发，指导最终窗口处理消息位置
 */
class AZER_EXPORT EventTarget {
 public:
  EventTarget();

  /**
   * 预处理消息，如果消息不按照流程继续下发，则返回 false
   */
  virtual bool PreProcessEvent(Event* event) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(EventTarget);
};
}  // namespace ui
}  // namespace azer
