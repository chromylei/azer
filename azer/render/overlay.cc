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

const azer::VertexDesc::Desc Overlay::kVertexDesc[] = {
  {"POSITION", 0, azer::kVec4},
  {"TEXCOORD", 0, azer::kVec2},
};

const int Overlay::kVertexDescNum = arraysize(Overlay::kVertexDesc);

bool Overlay::InitVertex(RenderSystem* rs) {
  // create vertex buffer
  vertex_desc_ptr_.reset(new VertexDesc(kVertexDesc, kVertexDescNum));
  VertexData data(vertex_desc_ptr_, kVertexNum);
  Vertex* ptr = (Vertex*)data.pointer();
  ptr->position = azer::Vector4(rect_.x(), rect_.bottom(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(0.0f, 0.0f);
  ptr++;
  ptr->position = azer::Vector4(rect_.x(), rect_.y(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(0.0f, 1.0f);
  ptr++;
  ptr->position = azer::Vector4(rect_.right(), rect_.bottom(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(1.0f, 0.0f);
  ptr++;

  ptr->position = azer::Vector4(rect_.right(), rect_.bottom(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(1.0f, 0.0f);
  ptr++;
  ptr->position = azer::Vector4(rect_.x(), rect_.y(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(0.0f, 1.0f);
  ptr++;
  ptr->position = azer::Vector4(rect_.right(), rect_.y(), 0.0f, 1.0f);
  ptr->texcoord = azer::Vector2(1.0f, 1.0f);

  vb_ptr_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), &data));
  if (!vb_ptr_.get()) {
    return false;
  }

  return true;
}

void Overlay::Render(Renderer* rs) {
  DCHECK (effect_ptr_.get() != NULL);
  effect_ptr_->Use(rs);
  rs->Draw(vb_ptr_.get(), azer::kTriangleList, 6);
}
}  // namespace azer
