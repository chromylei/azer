#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/window/native_message.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/point.h"

namespace azer {
namespace window {
class AZER_EXPORT NativeMouseEvent : public NativeEvent {
 public:
 protected:
};

class AZER_EXPORT NativeMouseMoveEvent : public NativeMouseEvent {
public:
};

class AZER_EXPORT NativeMouseKeydownEvent : public NativeMouseEvent {
};

class AZER_EXPORT NativeMouseKeyupEvent : public NativeMouseEvent {
};

class AZER_EXPORT NativeMouseClickEvent : public NativeMouseEvent {
 public:
 private:
};
}  // namespace window
}  // namespace azer
