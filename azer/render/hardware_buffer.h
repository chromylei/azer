#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class Renderer;

class AZER_EXPORT HardwareBufferData {
 public:
  HardwareBufferData() : data_(0), row_size_(-1), size_(-1) {
  }

  uint8* data_ptr() const { return data_;}
  int32 row_size() const { return row_size_;}
  int32 column_num() const { return column_num_;}
 private:
  uint8* data_;
  int32 row_size_;
  int32 column_num_;
  int32 size_;

  friend class HardwareBuffer;
  DISALLOW_COPY_AND_ASSIGN(HardwareBufferData);
};

typedef std::shared_ptr<HardwareBufferData> HardwareBufferDataPtr;

class AZER_EXPORT HardwareBuffer {
 public:
  HardwareBuffer() {}
  virtual ~HardwareBuffer() {}

  virtual HardwareBufferDataPtr map(MapType flags) = 0;
  virtual void unmap() = 0;
 protected:
  void SetLockDataPtr(void* ptr, HardwareBufferData* data) {
    data->data_ = (uint8*)ptr;
  }
  void SetLockDataRowSize(int32 size, HardwareBufferData* data) {
    data->row_size_ = size;
  }
  void SetLockDataColumnNum(int32 num, HardwareBufferData* data) {
    data->column_num_ = num;
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(HardwareBuffer);
};

class AZER_EXPORT AutoBufferLock {
 public:
  AutoBufferLock(HardwareBuffer* hbuffer, MapType flags, Renderer* renderer)
      : hbuffer_(hbuffer) {
    data_ = hbuffer_->map(flags);
  }
  ~AutoBufferLock() {
    if (data_.get()) {
      hbuffer_->unmap();
    }
  }
 private:
  HardwareBufferDataPtr data_;
  HardwareBuffer* hbuffer_;
  DISALLOW_COPY_AND_ASSIGN(AutoBufferLock);
};
}  // namespace azer
