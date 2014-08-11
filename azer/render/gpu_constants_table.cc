#include "azer/render/gpu_constants_table.h"
#include "azer/render/light.h"
#include "azer/base/align.h"

namespace azer {

GpuConstantsTable::GpuConstantsTable(int32 num, const Desc* desc) {
  int32 offset = 0;
  const Desc* curr = desc;
  for (int32 i = 0; i < num; ++i, ++curr) {
    const int32 size = GpuTableItemDescSize(*curr);
    const int32 total_size = size * desc->num;
    offset = curr->offset;
    DCHECK(offset != -1);
    constants_.push_back(Variable(*curr, size, total_size, offset));
    offset += total_size;
  }
  size_ = AZER_ALIGN_64(offset);
  data_.reset(new uint8[size_]);
}

void GpuConstantsTable::SetArrayItem(int32 idx, int32 arridx, const void* value,
                                     int32 size) {
  DCHECK_GT(constants_.size(), 0u);
  DCHECK(idx >= 0 && idx < constants_.size());
  DCHECK(arridx >= 0 && arridx < constants_[idx].desc.num);
  const Variable& variable = constants_[idx];
  DCHECK_EQ(size, variable.element_size);
  SetData(variable.offset + size * arridx, value, size);
}

void GpuConstantsTable::SetArrayMultiItem(int32 idx, int32 arridx, const void* value,
                                          int32 size) {
  DCHECK_GT(constants_.size(), 0u);
  DCHECK(idx >= 0 && idx < constants_.size());
  DCHECK(arridx >= 0 && arridx < constants_[idx].desc.num);
  const Variable& variable = constants_[idx];
  DCHECK_LE(variable.element_size * arridx + size, variable.size);
  SetData(variable.offset + size * arridx, value, size);
}

void GpuConstantsTable::SetValueWithOffset(int32 idx, int32 offset,
                                           const void* value, int32 size) {
  DCHECK_GT(constants_.size(), 0u);
  DCHECK(idx >= 0 && idx < constants_.size());
  const Variable& variable = constants_[idx];
  DCHECK_LE(size, variable.size);
  SetData(variable.offset + offset, value, size);
}

void GpuConstantsTable::SetLight(int32 idx, const Light& light) {
  return SetValue(idx, (void*)light.gpu_constant_data(), light.gpu_exchange_size());
}

void GpuConstantsTable::SetLightArrItem(int32 idx, int32 arridx,
                                        const Light& light) {
  DCHECK_LE(light.gpu_exchange_size(), constants_[idx].size);
  DCHECK_EQ(constants_[idx].element_size, light.gpu_exchange_size());
  return SetValueWithOffset(idx,
                            light.gpu_exchange_size() * arridx,
                            (void*)light.gpu_constant_data(),
                            light.gpu_exchange_size());
}

int32 GpuTableItemTypeSize(const GpuConstantsType::Type type) {
  switch(type) {
    case GpuConstantsType::kFloat: return sizeof(float);
    case GpuConstantsType::kVector2: return sizeof(azer::Vector2);
    case GpuConstantsType::kVector3: return sizeof(azer::Vector3);
    case GpuConstantsType::kVector4: return sizeof(azer::Vector4);
    case GpuConstantsType::kInt: return sizeof(uint32);
    case GpuConstantsType::kIntVec2: return sizeof(uint32) * 2;
    case GpuConstantsType::kIntVec3: return sizeof(uint32) * 3;
    case GpuConstantsType::kIntVec4: return sizeof(uint32) * 4;
    case GpuConstantsType::kMatrix3: return sizeof(azer::Matrix3);
    case GpuConstantsType::kMatrix4: return sizeof(azer::Matrix4);
    case GpuConstantsType::kSampler1D: return sizeof(uint32);
    case GpuConstantsType::kSampler2D: return sizeof(uint32);
    case GpuConstantsType::kSampler3D: return sizeof(uint32);
    case GpuConstantsType::kSamplerCube: return sizeof(uint32);
    case GpuConstantsType::kDirectionalLight:
      return Light::gpu_exchange_size(Light::kDirectionalLight);
    case GpuConstantsType::kPointLight:
      return Light::gpu_exchange_size(Light::kPointLight);
    case GpuConstantsType::kSpotLight:
      return Light::gpu_exchange_size(Light::kSpotLight);
    case GpuConstantsType::kSelfDefined:
      return -1;
    default:
      CHECK(false) << "No such GpuConstantType: " << (int32)type;
      return 0;
  }
}
}  // namespace azer
