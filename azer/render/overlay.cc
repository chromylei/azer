#include "azer/render/overlay.h"
#include "azer/render/render.h"

namespace azer {

void OverlayEffect::Use(Renderer* renderer) {
  DCHECK(texture_ptr_.get() != NULL);
  renderer->UseTexture(azer::kPixelStage, 0, texture_ptr_.get());
  UseConstantsTable(renderer);
  UseTechnique(renderer);
  // Effect::Use(renderer);
}

void Overlay::Render(Renderer* rs) {
  DCHECK (effect_ptr_.get() != NULL);
  effect_ptr_->Use(rs);
  rs->Draw(vb_ptr_.get(), azer::kTriangleList, 6);
}
}  // namespace azer
