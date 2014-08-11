#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/render/light.h"
#include "azer/scene/node.h"
#include "azer/scene/scene.h"
#include "azer/resources/file_system.h"

namespace azer {

class ResourceManager;

class AZER_EXPORT SceneManager {
 public:
  SceneManager(ResourceManager* mgr) : res_mgr_(mgr) {}

  /**
   * Note: SceneNodePtr wonnot be shared with the same name
   * because of scenenode may be used in many scene, if one changed,
   * all other changed. so we resue scene info to avoid loading same
   * scenenode mang timed
   */
  SceneNode::NodeInfoPtr GetSceneNodeInfo(const StringType& name);
  ScenePtr GetScene(const StringType& name, RenderSystem* rs);
  LightPtr GetLight(const StringType& path);
 private:
  typedef std::unordered_map<StringType, SceneNode::NodeInfoPtr> SceneNodeDict;
  SceneNodeDict scene_node_dict_;
  bool LoadSceneNode(const ResFilePath& path);
  SceneNode::NodeInfoPtr FindSceneNode(const StringType& name);
  bool AddSceneNode(const StringType& fullpath, SceneNode::NodeInfoPtr ptr);
  bool RecursiveCreateNode(SceneNode::NodeInfoPtr infoptr, SceneNode* parent,
                           RenderSystem* rs);

  typedef std::unordered_map<StringType, LightPtr> LightDict;
  bool LoadLight(const ResFilePath& path);
  LightPtr FindLight(const StringType& name);
  LightDict light_dict_;

  ResourceManager* res_mgr_;
  DISALLOW_COPY_AND_ASSIGN(SceneManager);
};

inline bool SceneManager::AddSceneNode(const StringType& fullpath,
                                          SceneNode::NodeInfoPtr ptr) {
  auto iter = scene_node_dict_.find(fullpath);
  if (iter == scene_node_dict_.end()) {
    scene_node_dict_[fullpath] = ptr;
    return true;
  } else {
    return false;
  }
}

inline SceneNode::NodeInfoPtr SceneManager::FindSceneNode(const StringType& name) {
  auto iter = scene_node_dict_.find(name);
  if (iter != scene_node_dict_.end()) {
    return iter->second;
  } else {
    return NULL;
  }
}


inline LightPtr SceneManager::FindLight(const StringType& name) {
  auto iter = light_dict_.find(name);
  if (iter != light_dict_.end()) {
    return iter->second;
  } else {
    return NULL;
  }
}

}  // namespace azer
