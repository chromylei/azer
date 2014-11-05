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

typedef std::shared_ptr<Event> EventPtr;k

}  // namespace ui
}  // namespace azer
