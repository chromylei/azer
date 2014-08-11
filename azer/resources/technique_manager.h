#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/base/string.h"
#include "azer/render/gpu_program.h"
#include "azer/render/technique.h"
#include "azer/resources/file_system.h"

namespace azer {

class RenderSystem;
class ShaderArray;
class ResourceManager;

class AZER_EXPORT TechniqueManager {
 public:
  TechniqueManager(ResourceManager* resmgr) : res_mgr_(resmgr) {
    DCHECK(resmgr != NULL);
  }

  TechniquePtr GetTechnique(RenderSystem* rs, const StringType& path,
                            ShaderType type = kStringShader);
 private:
  TechniquePtr FindTechnique(const StringType& name, const StringType& rs_name,
                             ShaderType type = kStringShader);
  bool LoadTechnique(RenderSystem* rs, const StringType& rs_name, ShaderType type);

  struct TechniqueResult {
    StringType vertex_desc;
    std::vector<StringType> program;

    TechniqueResult() {
      program.resize(kRenderPipelineStageNum);
    }
  };
  bool ParseTechnique(const char* buffer, int len, TechniqueResult* result);
  StringType DictionaryKey(const StringType& name, const StringType& rs_name,
                           ShaderType);
  typedef std::unordered_map<StringType, TechniquePtr> TechniqueDict;
  TechniqueDict technique_dict_;

  ResourceManager* res_mgr_;
  DISALLOW_COPY_AND_ASSIGN(TechniqueManager);
};

inline TechniquePtr TechniqueManager::FindTechnique(const StringType& path,
                                                    const StringType& rs_name,
                                                    ShaderType type) {
  StringType fullpath = std::move(DictionaryKey(path, rs_name, type));
  auto iter = technique_dict_.find(fullpath);
  if (iter != technique_dict_.end()) {
    return iter->second;
  } else {
    return NULL;
  }
}

inline StringType TechniqueManager::DictionaryKey(const StringType& path,
                                                  const StringType& rs_name,
                                                  ShaderType type) {
  StringType fullpath = path;
  fullpath.push_back(AZER_LITERAL('.'));
  fullpath.append(rs_name);
  fullpath.push_back(AZER_LITERAL('.'));
  fullpath.append(ShaderSuffix(type));
  return fullpath;
}
}  // namespace azer
