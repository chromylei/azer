#pragma once

#include <map>
#include <string>

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/scene/node.h"
#include "azer/base/render_export.h"

namespace azer {

class SceneNode;
class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class AZER_EXPORT Scene : public SceneNode {
public:
  Scene(const StringType& name, const Vector3& position, const Vector3& scale,
        const Quaternion& orientation);
  Scene(SceneNode* parent, const StringType& name, const Vector3& position,
        const Vector3& scale, const Quaternion& orientation);
  SceneNodePtr& find(const StringType& name);
  void UpdateHierarchy() { HierarchyTransform<SceneNode>::UpdateHierarchy();}
 protected:
  bool AddDescent(SceneNodePtr ptr);
  bool RemoveDescent(SceneNodePtr ptr);
  std::map<StringType, SceneNodePtr> nodemap_;

  SceneNodePtr empty_;
  friend class SceneNode;
  DISALLOW_COPY_AND_ASSIGN(Scene);
};

inline Scene::Scene(const StringType& name, const Vector3& position,
                    const Vector3& scale, const Quaternion& orientation)
    : SceneNode(NULL, name, kScene, position, scale, orientation) {
}

inline Scene::Scene(SceneNode* parent, const StringType& name,
                    const Vector3& position, const Vector3& scale,
                    const Quaternion& orientation)
    : SceneNode(parent, name, kScene, position, scale, orientation) {
  DCHECK_EQ(parent->type(), kScene);
}

inline SceneNodePtr& Scene::find(const StringType& name) {
  auto iter = nodemap_.find(name);
  if (iter != nodemap_.end()) {
    return iter->second;
  } else {
    return empty_;
  }
}
}  // namespace azer
