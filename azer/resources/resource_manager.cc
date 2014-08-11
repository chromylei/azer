#include "azer/resources/resource_manager.h"

#include "azer/math/math.h"
#include "azer/base/string.h"
#include "azer/resources/load_util.h"
#include "azer/resources/file_system.h"
#include "azer/resources/json_util.h"
#include "azer/render/render.h"
#include "base/json/json_reader.h"
#include "base/strings/string_piece.h"
#include "base/values.h"

#if !defined(AZER_COMMON_RESOURCE_PATH)
#define AZER_COMMON_RESOURCE_PATH (AZER_LITERAL("azer/resources/"))
#endif

namespace azer {
using detail::GetFloat;
using detail::GetInt;
using detail::GetVector3;
using detail::GetVector4;
using detail::GetQuaternion;

namespace {
::base::LazyInstance<ResourceManager> lazy_res_mgr_ = LAZY_INSTANCE_INITIALIZER;
}  // namespace

ResourceManager* ResourceManager::GetCommonResourceManager() {
  return lazy_res_mgr_.Pointer();
}

std::string ResourceManager::LoadGpuProgram(const ResFilePath& path) {
  FileContentPtr ptr = LoadFile(path);
  if (ptr.get() != NULL) {
    return std::string(ptr->data, ptr->length);
  } else {
    LOG(ERROR) << "Failed to get GpuProgram: " << path.value();
    return std::string("");
  }
}

const std::string& ResourceManager::GetGpuProgram(const StringType& name) {
  auto iter = gpu_program_dict_.find(name);
  if (iter != gpu_program_dict_.end()) {
    return iter->second;
  }

  std::string program = std::move(LoadGpuProgram(name));
  gpu_program_dict_[name] = program;
  return gpu_program_dict_[name];
}

}  // namespace azer
