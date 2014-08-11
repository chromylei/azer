#pragma once

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/tree_node.h"
#include "azer/base/string.h"
#include "azer/base/movable.h"
#include "azer/base/hierarchy_transform.h"
#include "azer/base/render_export.h"
#include "azer/render/cullable.h"
#include "azer/render/light.h"
#include "azer/render/mesh.h"
#include "azer/render/renderable.h"
#include "azer/render/axis_aligned_box.h"
#include "azer/resources/file_system.h"

namespace azer {

class Mesh;
class RenderSystem;

class Scene;
class SceneNode;
typedef std::shared_ptr<SceneNode> SceneNodePtr;

class AZER_EXPORT SceneNode : public HierarchyTransform<SceneNode>
                            , public Cullable {
 public:
  enum Type {
    kUnknown = -1,
    kLight = 1,
    kCamera,
    kMesh,
    kScene,
    kTypeNum = kScene,
  };

  struct NodeInfo {
    StringType name;
    StringType attach_object;
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;
    Type type;
    Vector3 aabb_min;
    Vector3 aabb_max;
    std::vector<StringType> children;

    NodeInfo()
        : scale(1.0f, 1.0f, 1.0f) {
    }
  };
  typedef std::shared_ptr<NodeInfo> NodeInfoPtr;

  // create child node
  SceneNodePtr CreateMeshNode(const StringType& name, const Vector3& pos,
                              const Vector3& scale, const Quaternion& orientation);
  SceneNodePtr CreateCameraNode(const StringType& name, const Vector3& pos,
                                const Vector3& scale, const Quaternion& orientation);
  SceneNodePtr CreateLightNode(const StringType& name, const Vector3& pos,
                               const Vector3& scale, const Quaternion& orientation);
  SceneNodePtr CreateNode(const StringType& name, SceneNode::Type type,
                          const Vector3& pos, const Vector3& scale,
                          const Quaternion& orientation);

  void set_visible(bool visible) { visible_ = visible;}
  bool is_visible() const { return visible_;}

  Type type() const { return type_;}

  // void Attach(RenderablePtr renderable);
  void AttachMesh(MeshPtr mesh);
  void AttachLight(LightPtr light);
  void Detach();

  RenderablePtr& GetAttachedRenderable() { return renderable_;}
  MeshPtr& GetAttachedMesh() { return mesh_;}
  Scene* root() { return root_;}

  // Cullable
  bool IsVisible(const Frustrum& frustrum) OVERRIDE;
 protected:
  SceneNode(SceneNode* parent, const StringType& name, Type type,
            const Vector3& position, const Vector3& scale,
            const Quaternion& orientation);

  const Type type_;
  bool visible_;

  // attached object
  RenderablePtr renderable_;
  MeshPtr mesh_;
  LightPtr light_;

  Scene* root_;
  AxisAlignedBox aabb_;

  friend class Scene;
  DISALLOW_COPY_AND_ASSIGN(SceneNode);
};

inline SceneNode::SceneNode(SceneNode* parent, const StringType& name, Type type,
                            const Vector3& position, const Vector3& scale,
                            const Quaternion& orientation)
    : HierarchyTransform(name, parent, position, scale, orientation)
    , type_(type)
    , visible_(true) {
  root_ = (parent == NULL) ? (Scene*)this : parent->root();
}

inline SceneNodePtr SceneNode::CreateMeshNode(const StringType& name,
                                              const Vector3& pos,
                                              const Vector3& scale,
                                              const Quaternion& orientation) {
  return CreateNode(name, kMesh, pos, scale, orientation);
}
inline SceneNodePtr SceneNode::CreateCameraNode(const StringType& name,
                                                const Vector3& pos,
                                                const Vector3& scale,
                                                const Quaternion& orientation) {
  return CreateNode(name, kCamera, pos, scale, orientation);
}

inline SceneNodePtr SceneNode::CreateLightNode(const StringType& name,
                                               const Vector3& pos,
                                               const Vector3& scale,
                                               const Quaternion& orientation) {
  return CreateNode(name, kLight, pos, scale, orientation);
  
}

/*
inline void SceneNode::Attach(RenderablePtr renderable) {
  DCHECK(type() == kRenderable);
  renderable_ = renderable;
}
*/

inline void SceneNode::AttachMesh(MeshPtr ptr) {
  DCHECK(type() == kMesh);
  mesh_ = ptr;
}

inline void SceneNode::AttachLight(LightPtr light) {
  DCHECK(type() == kLight);
  light_ = light;
}

typedef TreeNode<SceneNode>::Traverser SceneTraverser;
}  // namespace azer
