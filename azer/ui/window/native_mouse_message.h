#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/window/native_message.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/point.h"

namespace azer {
class AZER_EXPORT NativeMouseMessage : public NativeMessage {
 public:
 protected:
};

class AZER_EXPORT NativeMouseMoveMsg : public NativeMouseMessage {
public:
};

class AZER_EXPORT NativeMouseKeyDown : public NativeMouseMessage {
};

class AZER_EXPORT NativeMouseKeyUp : public NativeMouseMessage {
};

class AZER_EXPORT NativeMouseClickMsg : public NativeMouseMessage {
 public:
 private:
};
}  // namespace azer
