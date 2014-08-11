#include "azer/resources/scene_manager.h"

#include "azer/resources/json_util.h"
#include "azer/resources/resource_manager.h"

#include "base/json/json_reader.h"


namespace azer {
using detail::GetFloat;
using detail::GetVector3;
using detail::GetVector4;
namespace {
typedef std::vector<std::pair<StringType, LightPtr> > LightVec;
bool ParseSceneNodeInfo(const char* buffer, int len, const StringType& current,
                        std::vector<SceneNode::NodeInfoPtr>* vec);
bool ParseLight(const char* buffer, int len, LightVec* vec);
SceneNode::Type GetSceneNodeType(const std::string& scene_type);
StringType GetFullPath(const StringType& name, const StringType& current);
bool ParseSpotLight(const base::DictionaryValue* dict,Light::SpotLight* light);
bool ParseAttenuation(const base::DictionaryValue* pdict,
                      const std::string& path,  Light::Attenuation* atten);
bool ParseDirectionalLight(const base::DictionaryValue* dict,
                           Light::DirectionalLight* light);
bool ParsePointLight(const base::DictionaryValue* dict, Light::PointLight* light);

bool ValidAttenuation(const base::DictionaryValue* dict);
bool ValidDirLight(const base::DictionaryValue* dict);
bool ValidSpotLight(const base::DictionaryValue* dict);
bool ValidPointLight(const base::DictionaryValue* dict);
bool ValidScene(const base::DictionaryValue* dict);
}  // namespace

LightPtr SceneManager::GetLight(const StringType& name) {
  DCHECK(res_mgr_ != NULL);
  LightPtr ptr(FindLight(name));
  if (ptr.get()) return ptr;

  StringType path, package;
  if (!SplitPackage(name, &path, &package)) return NULL;
  FileContentPtr content_ptr(res_mgr_->LoadFile(ResFilePath(path)));
  if (content_ptr.get() == NULL) return NULL;
  if (!LoadLight(ResFilePath(path))) {
    return NULL;
  }

  return FindLight(name);
}

// scene
SceneNode::NodeInfoPtr SceneManager::GetSceneNodeInfo(const StringType& name) {
  DCHECK(res_mgr_ != NULL);
  SceneNode::NodeInfoPtr ptr = FindSceneNode(name);
  if (ptr.get()) {
    return ptr;
  }

  // get the package name
  StringType path, package;
  SplitPackage(name, &path, &package);
  FileContentPtr content_ptr(res_mgr_->LoadFile(ResFilePath(path)));
  if (content_ptr.get() == NULL) return NULL;

  std::vector<SceneNode::NodeInfoPtr> vec;
  ParseSceneNodeInfo(content_ptr->data, content_ptr->length, path, &vec);
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    StringType fullpath = path;
    fullpath.push_back(FILE_PATH_LITERAL(':'));
    fullpath.append((*iter)->name);
    if (!AddSceneNode(fullpath, *iter)) {
      LOG(ERROR) << "No scene node: " << name;
      return NULL;
    }
  }

  return FindSceneNode(name);
}

ScenePtr SceneManager::GetScene(const StringType& name, RenderSystem* rs) {
  DCHECK(res_mgr_ != NULL);
  SceneNode::NodeInfoPtr nodeptr = GetSceneNodeInfo(name);
  if (!nodeptr.get()) {
    LOG(ERROR) << "GetScene " << name << " failed.";
    return NULL;
  }

  if (nodeptr->type != SceneNode::kScene) {
    LOG(ERROR) << name << " not a scene";
    return NULL;
  }

  ScenePtr subscene_ptr(new Scene(nodeptr->name, nodeptr->position,
                                  nodeptr->scale, nodeptr->orientation));
  if (RecursiveCreateNode(nodeptr, subscene_ptr.get(), rs)) {
    return subscene_ptr;
  } else {
    return NULL;
  }
}

bool SceneManager::RecursiveCreateNode(SceneNode::NodeInfoPtr infoptr,
                                       SceneNode* parent, RenderSystem* rs) {
  DCHECK(res_mgr_ != NULL);
  if (!infoptr->attach_object.empty()) {
    if (infoptr->type == SceneNode::kMesh) {
      MeshPtr meshptr = res_mgr_->GetMesh(infoptr->attach_object, rs);
      if (meshptr.get()) {
        parent->AttachMesh(meshptr);
      } else {
        LOG(ERROR) << "No mesh: " << infoptr->attach_object;
      }
    } else if (infoptr->type == SceneNode::kLight) {
      LightPtr lightptr= GetLight(infoptr->attach_object);
      if (lightptr.get() == NULL) {
        parent->AttachLight(lightptr);
      } else {
        LOG(ERROR) << "No light: " << infoptr->attach_object;
      }
    } else {
      NOTREACHED() << "not implement";
    }
  }

  for (auto iter = infoptr->children.begin();
       iter != infoptr->children.end(); ++iter) {
    SceneNode::NodeInfoPtr child_ptr(GetSceneNodeInfo(*iter));
    if (child_ptr.get() == NULL) {
      LOG(ERROR) << "Cannot get scenenode: " << *iter;
      return NULL;
    }

    SceneNodePtr child = parent->CreateNode(child_ptr->name, child_ptr->type,
                                            child_ptr->position, child_ptr->scale,
                                            child_ptr->orientation);
    if (!RecursiveCreateNode(child_ptr, child.get(), rs)) {
      return NULL;
    }
  }

  return true;
}

bool SceneManager::LoadLight(const ResFilePath& path) {
  DCHECK(res_mgr_ != NULL);
  FileContentPtr ptr = res_mgr_->LoadFile(path);
  LightVec vec;
  if (!ParseLight((const char*)(ptr->data), ptr->length, &vec)) {
    LOG(ERROR) << "Failed to LoadLight from path: " << path.value();
    return false;
  }

  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    StringType fullpath = path.value();
    fullpath.push_back(FILE_PATH_LITERAL(':'));
    fullpath.append(iter->first);
    light_dict_[fullpath] = iter->second;
  }

  return true;
}

namespace {
bool ParseSceneNodeInfo(const char* buffer, int len, const StringType& current,
                        std::vector<SceneNode::NodeInfoPtr>* vec) {
  int json_error_code = 0;
  std::string json_error_desc;
  ::base::StringPiece piece(buffer, len);
  std::unique_ptr<::base::Value> root(
      ::base::JSONReader::ReadAndReturnError(piece,
                                             base::JSON_ALLOW_TRAILING_COMMAS,
                                             &json_error_code, &json_error_desc));
  ::base::ListValue* root_list = NULL;
  if (!root.get()) {
    LOG(ERROR) << "Failed to Parse Scene JSON: " << json_error_desc;
    return false;
  }
  if (!root->GetAsList(&root_list)) {
    return false;
  }

  StringType name;
  std::string type;
  std::vector<base::FilePath::StringType> texvec;
  for (int i = 0; i < root_list->GetSize(); ++i) {
    SceneNode::NodeInfoPtr info(new SceneNode::NodeInfo);
    const Value* value = NULL;
    if (!root_list->Get(i, &value)) return false;
    const ::base::DictionaryValue* dict = NULL;
    if (!value->GetAsDictionary(&dict)) return false;
    if (!ValidScene(dict)) return false;

    
    dict->GetString("name", &(info->name));
    dict->GetString("type", &type);
    info->type = GetSceneNodeType(type);
    dict->GetString("attach_object", &(info->attach_object));
    detail::GetVector3(dict, "position", &(info->position));
    detail::GetVector3(dict, "scale", &(info->scale));
    detail::GetQuaternion(dict, "ambient", &(info->orientation));
    detail::ParseAABB(dict, "aabb", &(info->aabb_min), &(info->aabb_max));

    const ::base::ListValue* children_list = NULL;
    if (dict->GetList("children", &children_list)) {
      if (children_list->GetSize() > 0 && info->type != SceneNode::kScene) {
          LOG(ERROR) << "Only subsceen or scene can has children";
          return false;
      }
      for (int cindex = 0; cindex < children_list->GetSize(); ++cindex) {
        if (!children_list->Get(cindex, &value)) return false;
        if (!value->GetAsString(&name)) return false;
        info->children.push_back(GetFullPath(name, current));
      }
    }

    vec->push_back(info);
  }

  return true;
}

bool ParseLight(const char* buffer, int len, LightVec* vec) {
  int json_error_code = 0;
  std::string json_error_desc;
  ::base::StringPiece piece(buffer, len);
  std::unique_ptr<::base::Value> root(
      ::base::JSONReader::ReadAndReturnError(piece,
                                             base::JSON_ALLOW_TRAILING_COMMAS,
                                             &json_error_code, &json_error_desc));
  ::base::ListValue* root_list = NULL;
  if (!root.get()) {
    LOG(ERROR) << "Failed to Parse Light JSON: " << json_error_desc;
    return false;
  }
  if (!root->GetAsList(&root_list)) {
    return false;
  }

  StringType name;
  std::string type;
  std::vector<base::FilePath::StringType> texvec;
  for (int i = 0; i < root_list->GetSize(); ++i) {
    const Value* value = NULL;
    if (!root_list->Get(i, &value)) return false;
    const ::base::DictionaryValue* dict = NULL;
    if (!value->GetAsDictionary(&dict)) return false;

    if (!dict->GetString("name", &name)) return false;
    if (!dict->GetString("type", &type)) return false;
    if (type == "directional") {
      Light::DirectionalLight dirlight;
      if (!ParseDirectionalLight(dict, &dirlight)) return false;
      vec->push_back(std::make_pair(name, LightPtr(new Light(dirlight))));
    } else if (type == "point") {
      Light::PointLight light;
      if (!ParsePointLight(dict, &light)) return false;
      vec->push_back(std::make_pair(name, LightPtr(new Light(light))));
    } else if (type == "spot") {
      Light::SpotLight spotlight;
      if (!ParseSpotLight(dict, &spotlight)) return false;
      vec->push_back(std::make_pair(name, LightPtr(new Light(spotlight))));
    } else {
      NOTREACHED();
      return false;
    }
  }

  return true;
}

inline StringType GetFullPath(const StringType& name, const StringType& current) {
  if (name.c_str()[0] != AZER_LITERAL(':')) {
    return name;
  } else {
    StringType path(current);
    path.append(name);
    return path;
  }
}

inline SceneNode::Type GetSceneNodeType(const std::string& scene_type) {
  if (scene_type == "light") return SceneNode::kLight;
  else if (scene_type == "camera") return SceneNode::kCamera;
  else if (scene_type == "mesh") return SceneNode::kMesh;
  else if (scene_type == "scene") return SceneNode::kScene;
  else return SceneNode::kUnknown;
}

inline bool ParseAttenuation(const base::DictionaryValue* pdict,
                             const std::string& path,
                             Light::Attenuation* atten) {
  const ::base::DictionaryValue* dict = NULL;
  if (!pdict->GetDictionary(path, &dict)) return false;
  if (!GetFloat(dict, "cvalue", &atten->cvalue)) return false;
  if (!GetFloat(dict, "linear", &atten->linear)) return false;
  if (!GetFloat(dict, "quad", &atten->expr)) return false;
  return true;
}

inline bool ParseDirectionalLight(const base::DictionaryValue* dict,
                                  Light::DirectionalLight* light) {
  if (!ValidDirLight(dict)) return false;
  if (!GetVector4(dict, "ambient", &light->ambient)) return false;
  if (!GetVector4(dict, "diffuse", &light->diffuse)) return false;
  if (!GetVector4(dict, "directional", &light->directional)) return false;
  return true;
}

inline bool ParsePointLight(const base::DictionaryValue* dict,
                            Light::PointLight* light) {
  if (!ValidPointLight(dict)) return false;
  if (!GetVector4(dict, "ambient", &light->ambient)) return false;
  if (!GetVector4(dict, "diffuse", &light->diffuse)) return false;
  if (!GetVector4(dict, "position", &light->position)) return false;
  if (!GetFloat(dict, "range", &light->range)) return false;
  if (!ParseAttenuation(dict, "attenuation", &light->attenuation)) return false;
  return true;
}

inline bool ParseSpotLight(const base::DictionaryValue* dict,
                           Light::SpotLight* light) {
  float degree;
  if (!ValidSpotLight(dict)) return false;
  if (!GetVector4(dict, "ambient", &light->ambient)) return false;
  if (!GetVector4(dict, "diffuse", &light->diffuse)) return false;
  if (!GetVector4(dict, "position", &light->position)) return false;
  if (!GetVector4(dict, "directional", &light->directional)) return false;
  if (!GetFloat(dict, "range", &light->range)) return false;
  if (!ParseAttenuation(dict, "attenuation", &light->attenuation)) return false;
  if (!GetFloat(dict, "cone", &degree)) return false;
  light->cone = Radians(Degree(degree));
  return true;
}

#if !defined(DISABLE_VALID_RESOURCE_JSON)
bool ValidAttenuation(const base::DictionaryValue* dict) {
  base::DictionaryValue::Iterator iter(*dict);
  for (; !iter.IsAtEnd(); iter.Advance()) {
    if (iter.key() == "cvalue") {
    } else if (iter.key() == "linear") {
    } else if (iter.key() == "quad") {
    } else {
      return false;
    }
  }

  return true;
}

bool ValidDirLight(const base::DictionaryValue* dict) {
  base::DictionaryValue::Iterator iter(*dict);
  for (; !iter.IsAtEnd(); iter.Advance()) {
    if (iter.key() == "name") {
    } else if (iter.key() == "type") {
    } else if (iter.key() == "ambient") {
    } else if (iter.key() == "diffuse") {
    } else if (iter.key() == "directional") {
    } else {
      LOG(ERROR) << "Unknown field " << iter.key() << " for dirlight";
      return false;
    }
  }

  return true;
}

bool ValidSpotLight(const base::DictionaryValue* dict) {
  base::DictionaryValue::Iterator iter(*dict);
  for (; !iter.IsAtEnd(); iter.Advance()) {
    if (iter.key() == "name") {
    } else if (iter.key() == "type") {
    } else if (iter.key() == "ambient") {
    } else if (iter.key() == "diffuse") {
    } else if (iter.key() == "position") {
    } else if (iter.key() == "directional") {
    } else if (iter.key() == "range") {
    } else if (iter.key() == "cone") {
    } else if (iter.key() == "attenuation") {
    } else {
      LOG(ERROR) << "Unknown field " << iter.key() << " for spotlight";
      return false;
    }
  }

  const ::base::DictionaryValue* atten_dict = NULL;
  if (dict->GetDictionary("attenuation", &atten_dict)) {
    return ValidAttenuation(atten_dict);
  } else {
    return true;
  }
}

bool ValidPointLight(const base::DictionaryValue* dict) {
  base::DictionaryValue::Iterator iter(*dict);
  for (; !iter.IsAtEnd(); iter.Advance()) {
    if (iter.key() == "name") {
    } else if (iter.key() == "type") {
    } else if (iter.key() == "ambient") {
    } else if (iter.key() == "diffuse") {
    } else if (iter.key() == "position") {
    } else if (iter.key() == "range") {
    } else if (iter.key() == "attenuation") {
    } else {
      LOG(ERROR) << "Unknown field " << iter.key() << " for pointlight";
      return false;
    }
  }

  const ::base::DictionaryValue* atten_dict = NULL;
  if (dict->GetDictionary("attenuation", &atten_dict)) {
    return ValidAttenuation(atten_dict);
  } else {
    return true;
  }
}

bool ValidScene(const base::DictionaryValue* dict) {
  std::string value;
  base::DictionaryValue::Iterator iter(*dict);
  for (; !iter.IsAtEnd(); iter.Advance()) {
    if (iter.key() == "name") {
    } else if (iter.key() == "position") {
    } else if (iter.key() == "scale") {
    } else if (iter.key() == "orientation") {
    } else if (iter.key() == "attach_object") {
    } else if (iter.key() == "type") {
    } else if (iter.key() == "children") {
    } else {
      LOG(ERROR) << "Unknown field " << iter.key() << " for pointlight";
      return false;
    }
  }

  return true;
}
#else
inline bool ValidAttenuation(const base::DictionaryValue* dict) { return true;}
inline bool ValidDirLight(const base::DictionaryValue* dict) { return true;}
inline bool ValidSpotLight(const base::DictionaryValue* dict) { return true;}
inline bool ValidPointLight(const base::DictionaryValue* dict) { return true;}
inline bool ValidScene(const base::DictionaryValue* dict) { return true;}
#endif
}  // namespace
}  // namespace azer

