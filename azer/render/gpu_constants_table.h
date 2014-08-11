#pragma once

#include <vector>

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"
#include "azer/render/material.h"

namespace azer {

class Light;
class Renderer;

class GpuConstantsType {
 public:
  enum Type {
    kFloat = 1,
    kVector2 = kVec2,
    kVector3 = kVec3,
    kVector4 = kVec4,
    kInt,
    kIntVec2,
    kIntVec3,
    kIntVec4,
    kUint,
    kUintVec2,
    kUintVec3,
    kUintVec4,
    kBool,
    kBoolVec2,
    kBoolVec3,
    kBoolVec4,
    kMatrix2,
    kMatrix3,
    kMatrix4,
    kSampler1D,
    kSampler2D,
    kSampler3D,
    kSamplerCube,
    kDirectionalLight,
    kPointLight,
    kSpotLight,
    kMaterial,
    kSelfDefined,
  };
};

AZER_EXPORT int32 GpuTableItemTypeSize(const GpuConstantsType::Type type);

class AZER_EXPORT GpuConstantsTable {
 public:
  struct Desc {
    char name[64];
    GpuConstantsType::Type type;
    int element_size;
    int32 num;
    uint32 offset;

    Desc(const char* n, GpuConstantsType::Type t, int32 off, int elenum)
        : type(t), element_size(-1),  num(elenum), offset(off) {
      strncpy(name, n, sizeof(name) - 1);
      element_size = GpuTableItemTypeSize(t);
    }

    Desc(const char* n, int32 off, int size, int elenum)
        : type(GpuConstantsType::kSelfDefined)
        , element_size(size), num(elenum), offset(off) {
      strncpy(name, n, sizeof(name) - 1);
    }

    Desc(const Desc& desc) {
      strncpy(this->name, desc.name, sizeof(name));
      this->type = desc.type;
      this->element_size = desc.element_size;
      this->num = desc.num;
      this->offset = desc.offset;
    }
  };

  virtual ~GpuConstantsTable() {}
  virtual void flush(Renderer*) = 0;

  // set value to gpu constants
  // vodi SetLight(int32 idx, const Light& light);
  void SetValue(int32 idx, const void* value, int32 size);
  void SetValueWithOffset(int32 idx, int32 offset, const void* value, int32 size);
  void SetArrayItem(int32 idx, int32 arridx, const void* value, int32 size);
  void SetArrayMultiItem(int32 idx, int32 arridx, const void* value, int32 size);
  void SetLight(int32 idx, const Light& light);
  void SetLightArrItem(int32 idx, int32 arridx, const Light& light);
  void SetMaterial(int32 idx, const Material& mtrl);
  void SetMatrix(int32 idx, const Matrix4* mtrl, int num);

  int32 offset(int32 index) const;
  int32 size() const { return size_;}
 protected:
  GpuConstantsTable(int32 num, const Desc* desc);

  struct Variable {
    Desc desc;
    int32 size;
    int32 element_size;
    int32 offset;
    void* extra;
    Variable(const Desc& d, int32 es, int32 s, int32 off)
        : desc(d), size(s), element_size(es), offset(off), extra(NULL) {}
  };

  /**
   * all try to modify data, should call this function
   */
  void SetData(int offset, const void* value, int32 size);
  std::vector<Variable> constants_;
  std::unique_ptr<uint8[]> data_;
  int32 size_;
  DISALLOW_COPY_AND_ASSIGN(GpuConstantsTable);
};

inline int32 GpuConstantsTable::offset(int32 index) const {
  DCHECK_GE(index, 0);
  DCHECK_LT(index, constants_.size());
  return constants_[index].offset;
}

inline void GpuConstantsTable::SetValue(int32 idx, const void* value, int32 size) {
  return SetValueWithOffset(idx, 0, value, size);
}

inline void GpuConstantsTable::SetMaterial(int32 idx, const Material& mtrl) {
  return SetValue(idx, (const void*)&(mtrl.attributes()),
                  sizeof(Material::material));
}

inline void GpuConstantsTable::SetMatrix(int32 idx, const Matrix4* matrices,
                                         int num) {
  return SetValue(idx, (const void*)matrices, sizeof(Matrix4) * num);
}

inline void GpuConstantsTable::SetData(int offset, const void* value, int32 size) {
  memcpy(data_.get() + offset, value, size);
}

typedef std::shared_ptr<GpuConstantsTable> GpuConstantsTablePtr;

inline int32 GpuTableItemDescSize(const GpuConstantsTable::Desc& desc) {
  if (desc.type == GpuConstantsType::kSelfDefined) {
    DCHECK_NE(desc.element_size, -1);
    return desc.element_size;
  } else {
    return GpuTableItemTypeSize(desc.type);
  }
}
}  // namespace
