#include "azer/resources/geometry_manager.h"

#include "azer/render/render.h"
#include "azer/resources/json_util.h"
#include "azer/resources/mesh.h"
#include "azer/resources/resource_manager.h"
#include "base/json/json_reader.h"
#include "base/strings/string_piece.h"
#include "base/values.h"

namespace azer {
namespace {
inline IndicesData::IndexType GetIndicesType(int32 indices_size) {
  switch (indices_size) {
    case 1: return IndicesData::kUint8;
    case 2: return IndicesData::kUint16;
    case 4: return IndicesData::kUint32;
    default: NOTREACHED(); return IndicesData::kUndefined;
  }
}
}  //  namespace

bool GeometryManager::LoadVertexDesc(const ResFilePath& path) {
  DCHECK(res_mgr_ != NULL);
  FileContentPtr ptr = res_mgr_->LoadFile(path);
  if (ptr.get() == NULL) {
    LOG(ERROR) << "Failed to Load path: " << path.value();
    return false;
  }

  detail::VertexDescVec vec;
  if (!detail::ParseVertexDesc((const char*)(ptr->data), ptr->length, &vec)) {
    LOG(ERROR) << "Failed to LoadVertexDesc from path: " << path.value();
    return false;
  }

  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    StringType fullpath = path.value();
    fullpath.push_back(FILE_PATH_LITERAL(':'));
    fullpath.append(iter->first);
    vertex_desc_dict_[fullpath] = iter->second;
  }

  return true;
}

VertexDescPtr GeometryManager::GetVertexDesc(const StringType& name) {
  DCHECK(res_mgr_ != NULL);
  VertexDescPtr ptr(FindVertexDesc(name));
  if (ptr.get()) return ptr;

  // get the package name
  StringType path, package;
  if (!SplitPackage(name, &path, &package)) return NULL;
  FileContentPtr content_ptr(res_mgr_->LoadFile(ResFilePath(path)));
  if (content_ptr.get() == NULL) return NULL;  
  if (!LoadVertexDesc(ResFilePath(path))) {
    LOG(ERROR) << "Cannot get VertexDesc: " << name;
    return NULL;
  }
  
  return FindVertexDesc(name);
}

MeshPtr GeometryManager::GetMesh(const StringType& name, RenderSystem* rs) {
  MeshPtr mesh(new Mesh);
  SubMeshPtr ptr = GetSubMesh(name, rs);
  if (ptr.get()) {
    mesh->Add(ptr);
    return mesh;
  } else {
    LOG(ERROR) << "Failed to get mesh: " << name;
    return NULL;
  }
}

SubMeshPtr GeometryManager::GetSubMesh(const StringType& name, RenderSystem* rs) {
  auto iter = mesh_dict_.find(name);
  if (iter != mesh_dict_.end()) {
    return iter->second;
  }
  SubMeshPtr ptr = LoadSubMesh(name, rs);
  mesh_dict_[name] = ptr;
  return ptr;
}

SubMeshPtr GeometryManager::LoadSubMesh(const ResFilePath& path, RenderSystem* rs) {
  DCHECK(res_mgr_ != NULL);
  FileContentPtr ptr = res_mgr_->LoadFile(path);
  detail::RawMesh raw_mesh;
  if (ptr.get() == NULL || 
      !detail::LoadMeshData((const char*)(ptr->data), ptr->length, &raw_mesh)) {
    LOG(ERROR) << "Failed to LoadMeshData from path: " << path.value();
    return false;
  }

  // get vertex desc
  VertexDescPtr desc_ptr = GetVertexDesc(
      ResFilePath(raw_mesh.head.vertex_desc_name).value());

  // create SubMesh
  Mesh::SubMeshPtr submesh_ptr(new Mesh::SubMesh);
  VertexDataPtr vertex_data(new VertexData(desc_ptr, raw_mesh.head.vertex_num));
  memcpy(vertex_data->pointer(), raw_mesh.vertices.get(),
         raw_mesh.head.vertex_num * raw_mesh.head.vertex_unit_size);
  submesh_ptr->vb_ptr.reset(rs->CreateVertexBuffer(
      VertexBuffer::Options(), vertex_data));

  // create indices
  if (raw_mesh.indices.get() != NULL) {
    int32 indices_size = raw_mesh.head.indices_num * raw_mesh.head.indices_unit_size;
    IndicesDataPtr id_ptr(new IndicesData(
        raw_mesh.head.indices_num, GetIndicesType(raw_mesh.head.indices_unit_size),
        IndicesData::kGPUMemory));
    memcpy(id_ptr->pointer(), raw_mesh.indices.get(), indices_size);
    submesh_ptr->ib_ptr.reset(
        rs->CreateIndicesBuffer(IndicesBuffer::Options(), id_ptr));
  }

  for (int i = 0; i < raw_mesh.head.group_num; ++i) {
    Mesh::Group group;
    detail::GroupAttr& group_attr = raw_mesh.groups.get()[i];
    group.first = group_attr.range.first;
    group.num = group_attr.range.num;
    group.type = (Mesh::GroupType)group_attr.type;
    group.primitive = group_attr.primitive;
    const char* mtrl_name = group_attr.mtrl;
    if (*mtrl_name != '\0') {

      group.mtrl_ptr = res_mgr_->GetMaterial(ResFilePath(mtrl_name).value(), rs);
      if (!group.mtrl_ptr.get()) {
        LOG(ERROR) << "No Material: " << mtrl_name
                   << " referred by mesh " << path.value();
      }
    }

    submesh_ptr->groups.push_back(group);
  }

  if (raw_mesh.head.bounding_volumn_type == BoundingVolumn::kAxisAlignedBox) {
    const float* data = (float*)raw_mesh.head.bounding_volumn_data;
    azer::Vector4 minimum(data[0], data[1], data[2], data[3]);
    azer::Vector4 maximum(data[4], data[5], data[6], data[7]);
    submesh_ptr->bounding.reset(new AxisAlignedBox(minimum, maximum));
  } else if (raw_mesh.head.bounding_volumn_type ==
             BoundingVolumn::kNoBoundingVolumn) {
  } else {
    NOTREACHED() << "No such Bounding volumn type";
  }

  // load material
  return submesh_ptr;
}
}  // namespace azer

