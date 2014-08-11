#include "azer/render/effect.h"

#include "base/logging.h"
#include "azer/render/render_system.h"
#include "azer/render/render_system_enum.h"

namespace azer {
Effect::Effect(RenderSystem* rs)
    : render_system_(rs) {
  DCHECK(NULL != render_system_);
  gpu_table_.resize(kRenderPipelineStageNum);
}

void Effect::Use(Renderer* renderer) {
  UseTexture(renderer);
  UseConstantsTable(renderer);
  UseTechnique(renderer);
}

void Effect::flush(Renderer* renderer) {
  DCHECK(NULL != render_system_);
  for (int i = (int)kVertexStage; i < (int)kPixelStage; ++i) {
    RenderPipelineStage stage = (RenderPipelineStage)i;
    GpuConstantsTable* table = gpu_table_[i].get();
    if (table != NULL) {
      table->flush(renderer);
    }
  }
}
}  // namespace azer
