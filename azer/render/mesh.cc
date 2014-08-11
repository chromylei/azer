#include "azer/render/mesh.h"

#include "azer/render/renderer.h"
#include "azer/render/effect.h"

namespace azer {

inline void Mesh::RenderGroup(Renderer* renderer, SubMesh* sub, const Group& group) {
  if (effect_ != NULL) {
    effect_->SetMaterial(group.mtrl_ptr);
    effect_->Use(renderer);
  }

  if (group.type == kIndices) {
    renderer->Render(sub->vb_ptr.get(), sub->ib_ptr.get(), group.primitive,
                     group.num, group.first);
  } else if (group.type == kVertex) {
    renderer->Render(sub->vb_ptr.get(), group.primitive, group.num, group.first);
  } else {
    NOTREACHED();
  }
}

void Mesh::RenderSubMesh(Renderer* renderer, SubMesh* sub) {
  for (auto iter = sub->groups.begin(); iter != sub->groups.end(); ++iter) {
    RenderGroup(renderer, sub, *iter);
  }
}

bool Mesh::IsVisible(const Frustrum& frustrum, const Matrix4& world) {
  for (auto iter = subs_.begin(); iter != subs_.end(); ++iter) {
    CHECK ((*iter)->bounding.get()) << "bounding box cannot be null";
    if ((*iter)->bounding->IsVisible(frustrum, world)) {
      return true;
    }
  }

  return false;
}

void Mesh::Render(Renderer* renderer) {
  for (auto iter = subs_.begin(); iter != subs_.end(); ++iter) {
    RenderSubMesh(renderer, (*iter).get());
  }
}

void Mesh::RenderBoundingVolumn(Renderer* renderer, const Matrix4& world,
                                const Camera& camera) {
  for (auto iter = subs_.begin(); iter != subs_.end(); ++iter) {
    if ((*iter)->bounding.get()) {
      (*iter)->bounding->Render(renderer, world, camera);
    }
  }
}

SubMeshPtr ComputeBoundSphere(Mesh* mesh) {
  return NULL;
}

SubMeshPtr ComputeBoundAABB(Mesh* mesh) {
  return NULL;
}

SubMeshPtr ComputeBoundOBB(Mesh* mesh) {
  return NULL;
}
}  // namespace azer
