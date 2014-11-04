#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/widget/event/event.h"

namespace azer {
namespace ui {

class AZER_EXPORT MouseEvent : public Event {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(MouseEvent);
};

class AZER_EXPORT MouseClickEvent : public MouseEvent {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(MouseClickEvent);
};

class AZER_EXPORT MouseDragEvent : public MouseEvent {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(MouseDragEvent);
};

class AZER_EXPORT MouseMoveEvent : public MouseEvent {
 public:
 protected:
  DISALLOW_COPY_AND_ASSIGN(MouseMoveEvent);
};
}  // namespace ui
}  // namespace azer
