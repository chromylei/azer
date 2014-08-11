#pragma once

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {
namespace detail { 
struct AZER_EXPORT MeshHead {
  char ident[4];
  int32 version;
  int32 reserve[4];

  // vertices
  char  vertex_desc_name[256];
  int32 vertex_unit_size;
  int32 vertex_num;
  int32 vertex_offset;

  // indices
  int32 indices_num;
  int32 indices_offset;
  int32 indices_unit_size;  // 1, 2, 4(8bit, 16bit, 32bit)

  // groups
  int32 group_num;
  int32 group_offset;

  int bounding_volumn_type;
  int bounding_volumn_data[128];
  uint8 padding[64];
};

struct AZER_EXPORT Range {
  int32 first;;
  int32 num;
};

struct AZER_EXPORT GroupAttr {
  int32 type;       // vertices(0) or indices(1)
  PrimitiveTopology primitive;
  Range range;
  char  mtrl[512];  // material path
};

struct AZER_EXPORT RawMesh {
  MeshHead head;
  std::unique_ptr<GroupAttr> groups;
  std::unique_ptr<uint8> vertices;
  std::unique_ptr<uint8> indices;
};

bool AZER_EXPORT LoadMeshData(const char* data, int len, RawMesh* mesh);
}  // namespace detail
}  // namespace azer
