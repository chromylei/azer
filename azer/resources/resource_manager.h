#pragma once

#include <string>
#include <unordered_map>

#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/resources/geometry_manager.h"
#include "azer/resources/material_manager.h"
#include "azer/resources/scene_manager.h"
#include "azer/resources/technique_manager.h"
#include "azer/resources/file_system.h"
#include "base/basictypes.h"
#include "base/lazy_instance.h"

namespace azer {

class RenderSystem;

/**
 * ResourceManager（资源管理类）
 * ResourceManager 并未设计到资源的释放，资源释放的过程可以通过将大场景分成子场景
 * 每一个（或者多个）子场景使用一个 ResourceManager, 当子场景不再使用时
 * 直接释放 ResourceManager 即可。
 */
class AZER_EXPORT ResourceManager {
 public:
  ResourceManager(FileSystemPtr filesystem);
  ResourceManager(FileSystemPtr filesystem,
                  FileSystemPtr backup_fs);
  ResourceManager(const std::vector<FileSystemPtr>& filesystems);
  ~ResourceManager();

  static ResourceManager* GetCommonResourceManager();

  /**
   * the following object shared by the same name
   */
  MaterialPtr GetMaterial(const StringType& name, RenderSystem *rs) {
    return material_mgr_.GetMaterial(name, rs);
  }
  TexturePtr GetTexture(const StringType& name, RenderSystem* rs) {
    return material_mgr_.GetTexture(name, rs);
  }

  const std::string& GetGpuProgram(const StringType& name);
  StringType GetString(const StringType& name);
  
  ScenePtr GetScene(const StringType& name, RenderSystem* rs) {
    return scene_mgr_.GetScene(name, rs);
  }
  LightPtr GetLight(const StringType& name) {
    return scene_mgr_.GetLight(name);
  }

  MeshPtr GetMesh(const StringType& name, RenderSystem* rs) {
    return geometry_manager_.GetMesh(name, rs);
  }
  SubMeshPtr GetSubMesh(const StringType& name, RenderSystem* rs) {
    return geometry_manager_.GetSubMesh(name, rs);
  }
  VertexDescPtr GetVertexDesc(const StringType& name) {
    return geometry_manager_.GetVertexDesc(name);
  }

  TechniquePtr GetTechnique(const StringType& path, RenderSystem* rs,
                            ShaderType type = kStringShader) {
    return technique_manager_.GetTechnique(rs, path, type);
  }

  bool GetAbsoluteFilePath(const ResFilePath& path, ::base::FilePath* absolutepath);
  FileContentPtr LoadFile(const ResFilePath& path);
 private:
  ResourceManager();
  
  std::string LoadGpuProgram(const ResFilePath& path);

  typedef std::unordered_map<StringType, std::string> ProgramDict;
  ProgramDict gpu_program_dict_;

  MaterialManager material_mgr_;
  GeometryManager geometry_manager_;
  TechniqueManager technique_manager_;
  SceneManager scene_mgr_;
  
  std::vector<FileSystemPtr> fs_ptr_vec_;

  friend struct ::base::DefaultLazyInstanceTraits<ResourceManager>;
  DISALLOW_COPY_AND_ASSIGN(ResourceManager);
};

inline ResourceManager::ResourceManager(FileSystemPtr filesystem)
    : material_mgr_(this)
    , geometry_manager_(this)
    , technique_manager_(this)
    , scene_mgr_(this) {
  fs_ptr_vec_.push_back(filesystem);
}

inline ResourceManager::ResourceManager(FileSystemPtr filesystem,
                                        FileSystemPtr backup_fs)
    : material_mgr_(this)
    , geometry_manager_(this)
    , technique_manager_(this)
    , scene_mgr_(this) {
  fs_ptr_vec_.push_back(filesystem);
  fs_ptr_vec_.push_back(backup_fs);
}

inline ResourceManager::ResourceManager(const std::vector<FileSystemPtr>& fs_vec)
    : material_mgr_(this)
    , geometry_manager_(this)
    , technique_manager_(this)
    , scene_mgr_(this)
    , fs_ptr_vec_(fs_vec) {
}

inline ResourceManager::ResourceManager()
    : material_mgr_(this)
    , geometry_manager_(this)
    , technique_manager_(this)
    , scene_mgr_(this) {
  fs_ptr_vec_.push_back(FileSystemPtr(FileSystem::GetDefaultFileSystem()));
}

inline ResourceManager::~ResourceManager() {
}

inline FileContentPtr ResourceManager::LoadFile(const ResFilePath& path) {
  for (auto iter = fs_ptr_vec_.begin(); iter != fs_ptr_vec_.end(); ++iter) {
    FileContentPtr ptr = (*iter)->LoadFile(path);
    if (ptr.get() != NULL) {
      return ptr;
    }
  }
  return NULL;
}

inline bool ResourceManager::GetAbsoluteFilePath(const ResFilePath& path,
                                                 ::base::FilePath* absolute_path) {
  for (auto iter = fs_ptr_vec_.begin(); iter != fs_ptr_vec_.end(); ++iter) {
    if ((*iter)->IsPathExists(path)) {
      *absolute_path = (*iter)->root().Append(path.value());
      return true;
    }
  }

  return false;
}
}  // namespace azer
