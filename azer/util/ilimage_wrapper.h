#pragma once

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render/render_system_enum.h"

namespace azer {
namespace util {
namespace detail {
class ilImageWrapper {
 public:
  ilImageWrapper()
      : width_(-1)
      , height_(-1)
      , image_id_((uint32)-1) {
  }

  ~ilImageWrapper();

  bool Create(int32 width, int32 height);
  bool Load(const ::base::FilePath& path);
  bool Load(uint8* data, int32 size, int32 type);
  bool Save(const ::base::FilePath& path);
  uint32 GetData(int32 x, int32 y);
  uint8* GetDataPtr();

  int32 height() const { return height_;}
  int32 width() const { return width_;}
  uint32 GetDataSize();
  bool InitFromData(const float* data);
  bool InitFromData(const uint32* data);
  bool InitFromData(const uint8* data);
 private:
  uint32 image_id_;
  int32 width_, height_;
  int32 bytes_per_pixel_;
  std::unique_ptr<uint8[]> data_;
  DISALLOW_COPY_AND_ASSIGN(ilImageWrapper);
};

}  // namespace detail
}  // namespace util
}  // namespace azer
