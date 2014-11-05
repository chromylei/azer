#pragma once

#include <memory>
#include <vector>

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/theme/painter.h"

namespace azer {
namespace ui {

/**
 * style group 代表一类具有相同风格的组
 */

class StyleGroup;

class AZER_WIDGET_EXPORT StyleGroupItem {
 public:
  virtual void OnStyleChangled(StyleGroup* group) = 0;
};

class AZER_WIDGET_EXPORT StyleGroup {
 public:
  StyleGroup();
  void AddItem(StyleGroupItem* item);
  PainterPtr& GetPainter(const std::string& name);

  void SetPainter(const std::string& name, PainterPtr ptr);
 private:
  std::vector<StyleGroupItem*> items_;
  PainterDict dict_;
  PainterPtr nullptr_;
  DISALLOW_COPY_AND_ASSIGN(StyleGroup);
};
}  // namespace ui
}  // namespace azer
