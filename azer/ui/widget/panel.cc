#include "azer/ui/widget/pannel.h"
#include "azer/render/surface.h"

#include "third_party/skia/include/core/SkColor.h"

namespace azer {
Panel::Panel()
    : render_system_(NULL)
    , color_(SkColorSetARGBInline(64, 84, 84, 84)) {
}

void Pannel::Create(const azer::Rect& rect, RenderSystem* render_system) {
  DCHECK(render_system != NULL);
  DCHECK(render_system_ == NULL && canvas_.get() == NULL && spirte_ == NULL);
}

/**
 */
void Pannel::Render() {
}
}  // namespace azer
