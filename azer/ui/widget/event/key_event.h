#pragma once

#include "azer/ui/widget/export.h"
#include "azer/ui/widget/event/event.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT KeyEvent : public Event {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(KeyEvent);
};

}  // namespace ui
}  // namespace azer
