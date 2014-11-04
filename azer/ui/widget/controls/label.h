#pragma once

#include "azer/base/render_export.h"
#include "azer/ui/widget/control.h"
#include "base/basictypes.h"
#include "azer/base/string.h"

namespace azer {
namespace ui {
class Context;

class AZER_EXPORT Label : public Control {
 public:
  Label(const gfx::Rect& rect, Widget* parent);
  Label(const StringType& str, const gfx::Rect& rect, Widget* parent);

  void SetText(const StringType& str) {text_ = str;}
  const StringType text() const { return text_;}
 public:
  virtual void Redraw(bool force) OVERRIDE;
 protected:
  StringType text_;
  DISALLOW_COPY_AND_ASSIGN(Label);
};
}  // namespace ui
}  // namespace azer
