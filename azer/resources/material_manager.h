#pragma once

#include <unordered_map>

#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/render/texture.h"
#include "azer/resources/file_system.h"
#include "azer/render/render_system.h"
#include "azer/render/material.h"
#include "base/basictypes.h"

namespace azer {

class ResourceManager;

class AZER_EXPORT MaterialManager {
 public:
  MaterialManager(ResourceManager* mgr)
      : res_mgr_(mgr) {
  }

  TexturePtr GetTexture(const StringType& name, RenderSystem* rs);
  MaterialPtr GetMaterial(const StringType& name, RenderSystem* rs);
 private:
  TexturePtr LoadTexture(const ResFilePath& path, RenderSystem* rs);

  typedef std::unordered_map<StringType, TexturePtr> TextureDict;
  TextureDict texture_dict_;

  /**
   * 从 json 格式当中加载 material
   * material 的 json 格式定义如下
   *
   * material : {
   *  'name': 'hello_material'
   *  'transparency': 0.3;
   *  'specular_power': 0.3;
   *  'optical_density' : 0.3;
   *  'illumination' : 0.3;
   *  'ambient' : [0.0, 1.0, 0.0, 0.0];
   *  'diffuse' : [0.0, 1.0, 0.0, 0.0];
   *  'diffuse_tex':    ['path'];
   *  'normal_tex':     ['path'];
   *  'specular_tex':   ['path'];
   *  'alpha_tex':      ['path'];
   * };
   */
  typedef std::unordered_map<StringType, MaterialPtr> MaterialDict;
  MaterialDict material_dict_;
  MaterialPtr FindMaterial(const StringType& name);
  bool LoadMaterial(const ResFilePath& path, RenderSystem* rs);

  RenderSystem* render_system_;
  ResourceManager* res_mgr_;
  DISALLOW_COPY_AND_ASSIGN(MaterialManager);
};

inline TexturePtr MaterialManager::GetTexture(const StringType& name,
                                              RenderSystem *rs) {
  auto iter = texture_dict_.find(name);
  if (iter != texture_dict_.end()) {
    return iter->second;
  }
  
  return LoadTexture(ResFilePath(name), rs);
}

inline MaterialPtr MaterialManager::FindMaterial(const StringType& name) {
  auto iter = material_dict_.find(name);
  if (iter != material_dict_.end()) {
    return iter->second;
  } else {
    return NULL;
  }
}

}  // namespace azer
