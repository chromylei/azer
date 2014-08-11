#include "azer/scene/scene.h"
#include "azer/scene/node.h"

namespace azer {

bool Scene::AddDescent(SceneNodePtr ptr) {
  const StringType& name = ptr->node_name();
  auto iter = nodemap_.find(name);
  if (iter == nodemap_.end()) {
    nodemap_[name] = ptr;
    return true;
  } else {
    return false;
  }
}

bool Scene::RemoveDescent(SceneNodePtr ptr) {
  const StringType& name = ptr->node_name();
  auto iter = nodemap_.find(name);
  if (iter != nodemap_.end()) {
    nodemap_.erase(iter);
    return true;
  } else {
    return false;
  }
}
}  // namespace azer
