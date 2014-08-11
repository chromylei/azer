#pragma once

#include <string>
#include <unordered_map>

#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/render/mesh.h"
#include "azer/render/vertex_data.h"
#include "azer/resources/file_system.h"
#include "base/basictypes.h"
#include "base/logging.h"

namespace azer {

class RenderSystem;
class ResourceManager;

class AZER_EXPORT GeometryManager {
 public:
  GeometryManager(ResourceManager* mgr) : res_mgr_(mgr) {
    DCHECK(NULL != mgr);
  }

  MeshPtr GetMesh(const StringType& name, RenderSystem* rs);
  SubMeshPtr GetSubMesh(const StringType& name, RenderSystem* rs);
  VertexDescPtr GetVertexDesc(const StringType& name);
 private:
  ResourceManager* res_mgr_;
  SubMeshPtr LoadSubMesh(const ResFilePath& path, RenderSystem* rs);

  typedef std::unordered_map<StringType, VertexDescPtr> VertexDescDict;
  bool LoadVertexDesc(const ResFilePath& path);
  VertexDescPtr FindVertexDesc(const StringType& path);
  VertexDescDict vertex_desc_dict_;

  typedef std::unordered_map<StringType, SubMeshPtr> SubMeshDict;
  SubMeshDict mesh_dict_;

  typedef std::unordered_map<StringType, MeshPtr> MeshDict;

  DISALLOW_COPY_AND_ASSIGN(GeometryManager);
};

inline VertexDescPtr GeometryManager::FindVertexDesc(const StringType& name) {
  auto iter = vertex_desc_dict_.find(name);
  if (iter != vertex_desc_dict_.end()) {
    return iter->second;
  } else {
    return NULL;
  }
}

}  // namespace azer
