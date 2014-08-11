#include "azer/render/vertex_data.h"

#include "base/logging.h"
#include "azer/math/math.h"

namespace azer {

VertexDesc::VertexDesc(const Desc* desc, int desc_num)
    : vertex_size_(0) {
  init(desc, desc_num);
}

VertexDesc::~VertexDesc() {
}

namespace {
inline int32 VertexTypeSize(DataFormat type) {
  switch (type) {
    case kScalar: return sizeof(float);
    case kVec2: return sizeof(azer::Vector2);
    case kVec3: return sizeof(azer::Vector3);
    case kVec4: return sizeof(azer::Vector4);
    case kInt: return sizeof(int);
    case kIntVec2: return sizeof(int) * 2;
    case kIntVec3: return sizeof(int) * 3;
    case kIntVec4: return sizeof(int) * 4;
    case kUint: return sizeof(uint32);
    case kUintVec2: return sizeof(uint32) * 2;
    case kUintVec3: return sizeof(uint32) * 3;
    case kUintVec4: return sizeof(uint32) * 4;
    case kBool: return sizeof(bool);
    case kBoolVec2: return sizeof(bool) * 2;
    case kBoolVec3: return sizeof(bool) * 3;
    case kBoolVec4: return sizeof(bool) * 4;
    case kRGBA: return sizeof(azer::Vector4);
    default:
      DCHECK(false) <<" No VertexDesc::Type: " << type;
      return 0;
  }
}

int32 calc_vertex_desc_size(const VertexDesc::Desc* desc) {
  // TODO(yanglei), align not support
  return VertexTypeSize(desc->type);
}
}

void VertexDesc::init(const Desc* desc, int desc_num) {
  DCHECK_EQ(offsets_idx_.size(), 0u);
  uint32 offset = 0;
  const Desc* cur = desc;
  offsets_idx_.resize(desc_num);
  for (int i = 0; i < desc_num; ++i, ++cur) {
    offsets_idx_[i] = offset;
    offset += calc_vertex_desc_size(cur);
  }

  vertex_size_ = offset;

  desc_.reset(new Desc[desc_num]);
  memcpy(desc_.get(), desc, sizeof(Desc) * desc_num);
}

std::ostream& operator << (std::ostream& os, const azer::VertexDesc& vertex_data) {
  os << "Vertex Data: [" << std::endl;
  int desc_num = static_cast<int>(vertex_data.offsets_idx_.size());
  for (int i = 0; i < desc_num; ++i) {
    const azer::VertexDesc::Desc& desc = vertex_data.desc_.get()[i];
    os << "\t" << desc.name;
  }
  os << "]" << std::endl;
      
  return os;
}

}  // namespace azer

