#include "azer/resources/mesh.h"
#include "base/logging.h"

namespace azer {
namespace detail {
bool LoadMeshData(const char* data, int len, RawMesh* mesh) {
  DCHECK(mesh != NULL);
  DCHECK(mesh->groups.get() == NULL);
  DCHECK(mesh->vertices.get() == NULL);
  DCHECK(mesh->indices.get() == NULL);

  MeshHead* head = &mesh->head;
  const char* ptr = data;
  memcpy((void*)head, ptr, sizeof(MeshHead));
  ptr += sizeof(MeshHead);

  const int vertex_size = head->vertex_unit_size * head->vertex_num;
  ptr = data + head->vertex_offset;
  DCHECK(ptr + vertex_size <= data + len);
  mesh->vertices.reset(new uint8[vertex_size]);
  memcpy(mesh->vertices.get(), ptr, vertex_size);

  const int indices_size = head->indices_unit_size * head->indices_num;
  if (indices_size > 0) {
    ptr = data + head->indices_offset;
    DCHECK(ptr + indices_size < data + len);
    mesh->indices.reset(new uint8[indices_size]);
    memcpy(mesh->indices.get(), ptr, indices_size);
  }

  mesh->groups.reset(new GroupAttr[head->group_num]);
  const Range* range_ptr = (const Range*)(data + head->group_offset);
  memcpy(mesh->groups.get(), range_ptr, sizeof(GroupAttr) * head->group_num);

  return true;
}
}  // namespace detail
}  // namespace azer
