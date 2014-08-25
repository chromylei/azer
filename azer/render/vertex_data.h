#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class VertexData;
class VertexDesc;
typedef std::shared_ptr<VertexData> VertexDataPtr;
typedef std::shared_ptr<VertexDesc> VertexDescPtr;

class AZER_EXPORT VertexDesc {
 public:
  /**
   * 对定点结构的定义
   * 参考 D3D11_INPUT_ELEMENT_DESC(Direct3D 11)
   * 参考 Opengl glAttributePointer()
   */
  struct Desc {
    char name[64];          // semantic_name
    int semantic_index;   // semantic_index
    DataFormat type;        // type
    bool aligned;           // aligned
  };

  VertexDesc(const Desc* desc, int desc_num);
  ~VertexDesc();

  /**
   * calc offset by name or index
   */
  int32 offset(const int32 index) const {
    DCHECK_LT(index, offsets_idx_.size());
    return offsets_idx_[index];
  }

  /**
   * size of single vertex 
   */
  int32 stride() const { return vertex_size_;}
  int32 element_num() const {
    DCHECK_GT(offsets_idx_.size(), 0u);
    return static_cast<int32>(offsets_idx_.size());
  }
  const Desc* descs() const { return desc_.get(); }

  /**
   * 工具函数，仅仅打印 VertexDesc 的 Desc
   */
  friend std::ostream& operator << (std::ostream& os, const VertexDesc& data);

  // debug function
  std::string dump_vertex_data() const;
 private:
  void init(const Desc* desc, int desc_num);

  /**
   * 此函数用于根据 strip 跟新 VertexDesc
   * 当用户调用 Map 时，GPU 的数据与 CPU 的数据，内存很有可能是
   * 不匹配的，此函数将根据 GPU 返回的长度重新计算 Desc
   */
  void ReCalc(int strip);
  std::unique_ptr<Desc[]> desc_;
  int32 vertex_size_;

  typedef std::vector<int32> OffsetIndex;
  OffsetIndex offsets_idx_;

  DISALLOW_COPY_AND_ASSIGN(VertexDesc);
};

class AZER_EXPORT VertexData {
 public:
  enum MemoryMode {
    kMainMemory,
    kGPUMemr,
  };

  VertexData(const VertexDescPtr& desc, int vertex_num);

  /**
   * 枚举方法
   * 如果对 next 的参数为 NULL， 它将返回第一个定点
   * 当 next() 返回 NULL 时， 表明已经到达最后一个定点
   */
  uint8* next(uint8* current) const;
  uint8* pointer() const { return data_.get();}
  void CopyFrom(uint8* data, uint32 size);

  int32 vertex_num() const {  return vertex_num_;}
  int32 element_num() const {
    DCHECK(desc_ptr_.get() != NULL);
    return desc_ptr_->element_num();
  }

  int32 stride() const {
    DCHECK(desc_ptr_.get() != NULL);
    return desc_ptr_->stride();
  }

  const VertexDescPtr& desc() const {
    DCHECK(desc_ptr_.get() != NULL);
    return desc_ptr_;
  }

  /**
   * whole buffer sizef of vertex data
   */
  int32 buffer_size() const {
    return vertex_num() * stride();
  }

  void reset() { data_.reset();}
 private:
  int32 vertex_num_;
  std::unique_ptr<uint8[]> data_;
  VertexDescPtr desc_ptr_;
};

// class VertexData
inline VertexData::VertexData(const VertexDescPtr& desc, int vertex_num)
    : desc_ptr_(desc)
    , vertex_num_(vertex_num) {
  data_.reset(new uint8[vertex_num_ * stride()]);
}

inline uint8* VertexData::next(uint8* current) const {
  DCHECK(current != NULL);
  DCHECK_EQ((current - pointer()) % stride(), 0);
  uint8* next_ptr = current + stride();
  return (next_ptr < pointer() + buffer_size()) ? next_ptr : NULL;
}

inline void VertexData::CopyFrom(uint8* dataptr, uint32 datasize) {
  DCHECK(dataptr != NULL);
  DCHECK(pointer() != NULL);
  DCHECK_LE(datasize, buffer_size());
  memcpy(pointer(), dataptr, datasize);
}
}  // namespace azer


