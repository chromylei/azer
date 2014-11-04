#pragma once

#include <memory>
#include "base/basictypes.h"
#include "azer/ui/widget/control.h"
#include "azer/ui/widget/panel.h"

namespace azer {
namespace ui {

class AZER_EXPORT Button : public Control {
 public:
  enum {
    kPressDown,
    kPressUp,
    kPressing,
  };
 private:
  DISALLOW_COPY_AND_ASSIGN(Button);
};

class AZER_EXPORT RadioButton : public Button {
 public:
 private:
  DISALLOW_COPY_AND_ASSIGN(RadioButton);
};

typedef std::shared_ptr<RadioButton> RadioButtonPtr;

class AZER_EXPORT RadioButtonGroup {
 public:
 private:
  std::vector<RadioButtonPtr> buttons_;
  DISALLOW_COPY_AND_ASSIGN(RadioButtonGroup);
};

class AZER_EXPORT Checkbox : public Button {
 public:
 private:
  DISALLOW_COPY_AND_ASSIGN(Checkbox);
};
typedef std::shared_ptr<Checkbox> CheckboxPtr;

class AZER_EXPORT ImageButton : public Button {
 public:
 private:
  DISALLOW_COPY_AND_ASSIGN(ImageButton);
};

}  // namespace ui 
}  // namespace azer
