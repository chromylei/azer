#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class Image {
 public:
  Image(int width, int height, DataFormat format = kRGBA8);

  int width() const;
  int height() const;
  uint8* data();
  const uint8* data() const;
  int32 data_size() const;
  int32 unit_size() const { return sizeof_dataformat(format_);}
  DataFormat format() const { return format_;}
 private:
  int32 sizeof_dataformat(DataFormat format) const;
  std::unique_ptr<uint8> data_;
  const int32 width_;
  const int32 height_;
  const DataFormat format_;
  DISALLOW_COPY_AND_ASSIGN(Image);
};

inline Image::Image(int32 width, int32 height, DataFormat format)
    : width_(width)
    , height_(height)
    , format_(format) {
  data_.reset(new uint8[sizeof_dataformat(format_) * width * height]);
}

inline int Image::width() const {
  DCHECK(data_.get() != NULL);
  return width_;
}
inline int Image::height() const {
  DCHECK(data_.get() != NULL);
  return height_;
}

inline int32 Image::data_size() const {
  DCHECK(data_.get() != NULL);
  return width_ * height_ * sizeof_dataformat(format_);
}

inline uint8* Image::data() {
  DCHECK(data_.get() != NULL);
  return data_.get();
}

inline const uint8* Image::data() const {
  DCHECK(data_.get() != NULL);
  return data_.get();
}

inline int32 Image::sizeof_dataformat(DataFormat format) const {
  switch (format) {
    case kRGBA8:
    case kRGBAn8:
      return (int32)sizeof(uint32);
    case kRGBA32:
    case kRGBAn32:
      return (int32)sizeof(uint32) * 4;
    case kRGBAf:
      return (int32)sizeof(float) * 4;
    default:
      NOTREACHED();
      return -1;
  }
}
}  // namespace azer
