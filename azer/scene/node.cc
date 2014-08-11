#include "azer/scene/node.h"

#include "azer/scene/scene.h"
#include "azer/render/render.h"
#include "azer/render/frustrum.h"
#include "azer/math/math.h"

namespace azer {
SceneNodePtr SceneNode::CreateNode(const StringType& name, SceneNode::Type type,
                                   const Vector3& pos, const Vector3& scale,
                                   const Quaternion& orientation) {
  DCHECK(this->type() == kScene);
  if (type != kScene) {
    SceneNodePtr ptr(new SceneNode(this, name, type, pos, scale, orientation));
    TreeNode<SceneNode>::AddChild(ptr.get());
    if (root_->AddDescent(ptr)) {
      return ptr;
    } else {
      return NULL;
    }
  } else {
    SceneNodePtr ptr(new Scene(this, name, pos, scale, orientation));
    TreeNode<SceneNode>::AddChild(ptr.get());
    if (root_->AddDescent(ptr)) {
      return ptr;
    } else {
      return NULL;
    }
  }
}

bool SceneNode::IsVisible(const Frustrum& frustrum) {
  if (type() == kMesh && mesh_.get()) {
    const Matrix4& world = GetWorldMatrix();
    return mesh_->IsVisible(frustrum, world);
  } else if (type() == kScene && !aabb_.IsNull()) {
  }

  return true;
}
}  // namespace azer
