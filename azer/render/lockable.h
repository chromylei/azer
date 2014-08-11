#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class Renderer;

class AZER_EXPORT LockData {
 public:
  LockData() : data_(0), row_size_(-1), size_(-1) {
  }

  uint8* data_ptr() const { return data_;}
  int32 row_size() const { return row_size_;}
  int32 column_num() const { return column_num_;}
 private:
  uint8* data_;
  int32 row_size_;
  int32 column_num_;
  int32 size_;

  friend class Lockable;
  DISALLOW_COPY_AND_ASSIGN(LockData);
};

typedef std::shared_ptr<LockData> LockDataPtr;

class AZER_EXPORT Lockable {
 public:
  Lockable() {}
  virtual ~Lockable() {}

  virtual LockDataPtr map(MapType flags) = 0;
  virtual void unmap() = 0;
 protected:
  void SetLockDataPtr(void* ptr, LockData* data) {
    data->data_ = (uint8*)ptr;
  }
  void SetLockDataRowSize(int32 size, LockData* data) {
    data->row_size_ = size;
  }
  void SetLockDataColumnNum(int32 num, LockData* data) {
    data->column_num_ = num;
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(Lockable);
};

class AZER_EXPORT AutoBufferLock {
 public:
  AutoBufferLock(Lockable* lockable, MapType flags, Renderer* renderer)
      : lockable_(lockable) {
    data_ = lockable_->map(flags);
  }
  ~AutoBufferLock() {
    if (data_.get()) {
      lockable_->unmap();
    }
  }
 private:
  LockDataPtr data_;
  Lockable* lockable_;
  DISALLOW_COPY_AND_ASSIGN(AutoBufferLock);
};
}  // namespace azer
