#include "azer/azer.h"
#include "azer/resources/mesh.h"
#include "base/base.h"
#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "sandbox/rastertek/d3d11/common/load.h"

using azer::detail::MeshHead;
using azer::detail::GroupAttr;;

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  std::vector<Vertex> vertices = std::move(
      loadModel(base::FilePath(::base::UTF8ToWide(argv[1]))));

  uint32 total_size = sizeof(MeshHead) + sizeof(GroupAttr)
      + sizeof(Vertex) * vertices.size();
  std::unique_ptr<uint8[]> data(new uint8[total_size]);
  memset(data.get(), 0, total_size);
  uint8* ptr = data.get();
  MeshHead* head = (MeshHead*)ptr;
  memcpy(head->ident, "afxm", 4);
  head->version = 0;
  strcpy(head->vertex_desc_name, "vdesc/base.vdesc:base1");
  head->vertex_unit_size = sizeof(Vertex);
  head->vertex_num = (int32)vertices.size();
  head->vertex_offset = sizeof(MeshHead) +  sizeof(GroupAttr);

  head->group_num = 1;
  head->group_offset = sizeof(MeshHead);

  GroupAttr* groups = (GroupAttr*)(ptr + head->group_offset);
  groups->type = azer::Mesh::kVertex;
  groups->range.first = 0;
  groups->range.num = vertices.size();
  groups->primitive = azer::kTriangleList;
  strcpy(groups->mtrl, argv[2]);
  memcpy(ptr + head->vertex_offset, &vertices[0], sizeof(Vertex) * vertices.size());

  return file_util::WriteFile(base::FilePath(FILE_PATH_LITERAL("a.afxm")),
                              (const char*)data.get(), total_size);
}
