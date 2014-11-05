#include "azer/ui/widget/theme/style_group.h"

namespace azer {
namespace ui {
StyleGroup::StyleGroup() {
}

void StyleGroup::AddItem(StyleGroupItem* item) {
  items_.push_back(item);
  item->OnStyleChangled(this);
}

PainterPtr& StyleGroup::GetPainter(const std::string& name) {
  auto iter = dict_.find(name);
  if (iter != dict_.end()) {
    return iter->second;
  } else {
    return nullptr_;
  }
}

void StyleGroup::SetPainter(const std::string& name, PainterPtr ptr) {
  dict_[name] = ptr;
}
}  // namespace ui
}  // namespace azer
