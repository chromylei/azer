#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/vertex_data.h"
#include "azer/render/hardware_buffer.h"

namespace azer {
class AZER_EXPORT IndicesData {
 public:
  enum IndexType {
    kUndefined = 0,
    kUint8,
    kUint16,
    kUint32,
  };

  IndicesData(int num, IndexType type)
      : type_(type), size_(0) {
    size_ = num * unit_size();
    num_ = num;
    data_.reset(new uint8[size_]);
  }

  int32 size() const { return size_;}
  const uint8* pointer() const {
    DCHECK(data_.get() != NULL);
    return data_.get();
  }

  uint8* pointer() {
    DCHECK(data_.get() != NULL);
    return data_.get();
  }
  IndexType type() const { return type_;}
  int32 num() const { return num_;}
  int32 unit_size() const;

  void reset() { data_.reset();}
 protected:
  IndexType type_;
  int32 size_;
  int32 num_;
  std::unique_ptr<uint8[]> data_;
  DISALLOW_COPY_AND_ASSIGN(IndicesData);
};

typedef std::shared_ptr<IndicesData> IndicesDataPtr;

class AZER_EXPORT IndicesBuffer : public HardwareBuffer {
 public:
  struct Options {
    GraphicBuffer::Usage usage;
    CPUAccess cpu_access;  // defined render_system
    Options()
        : usage(GraphicBuffer::kDefault)
        , cpu_access(kCPUNoAccess) {
    }
  };

  IndicesBuffer(const Options& opt)
      : options_(opt)
      , indices_num_(-1)
      , type_(IndicesData::kUndefined) {
  }

  virtual ~IndicesBuffer() {}

  virtual HardwareBufferDataPtr map(MapType flags) = 0;
  virtual void unmap() = 0;
  IndicesData::IndexType type() const { return type_;}

  const Options& options() const { return options_;}
  int32 indices_num() const { return indices_num_;}
 protected:
  const Options options_;
  int32 indices_num_;
  IndicesData::IndexType type_;
  DISALLOW_COPY_AND_ASSIGN(IndicesBuffer);
};

inline int32 IndicesData::unit_size() const {
  switch (type()) {
    case kUint8: return (int32)sizeof(uint8);
    case kUint16: return (int32)sizeof(uint16);
    case kUint32: return (int32)sizeof(uint32);
    default: CHECK(false); return 0;
  }
}

typedef std::shared_ptr<IndicesBuffer> IndicesBufferPtr;
}  // namespace azer
