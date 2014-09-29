#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class AZER_EXPORT ImageData {
 public:
  ImageData(int width, int height, DataFormat format = kRGBAn8);

  int width() const;
  int height() const;
  uint8* data();
  const uint8* data() const;

  uint32 pixel(int x, int y) const;
  void set_pixel(int x, int y, uint32 v);
  int32 data_size() const;
  int32 unit_size() const { return sizeof_dataformat(format_);}
  DataFormat format() const { return format_;}

  // sample functions
  // tu and tv [0, 1)
  Vector4 BoxSample(float tu, float tv) const;

  static ImageData* Load(const ::base::FilePath& path);
  static ImageData* Load(const ::base::FilePath::StringType& path);
 private:
  int32 sizeof_dataformat(DataFormat format) const;
  std::unique_ptr<uint8> data_;
  const int32 width_;
  const int32 height_;
  const DataFormat format_;
  DISALLOW_COPY_AND_ASSIGN(ImageData);
};

typedef std::shared_ptr<ImageData> ImageDataPtr;

inline ImageData::ImageData(int32 width, int32 height, DataFormat format)
    : width_(width)
    , height_(height)
    , format_(format) {
  uint32 size = data_size();
  data_.reset(new uint8[size]);
}

inline int ImageData::width() const {
  DCHECK(data_.get() != NULL);
  return width_;
}
inline int ImageData::height() const {
  DCHECK(data_.get() != NULL);
  return height_;
}

inline int32 ImageData::data_size() const {
  return width_ * height_ * sizeof_dataformat(format_);
}

inline uint8* ImageData::data() {
  DCHECK(data_.get() != NULL);
  return data_.get();
}

inline const uint8* ImageData::data() const {
  DCHECK(data_.get() != NULL);
  return data_.get();
}

inline int32 ImageData::sizeof_dataformat(DataFormat format) const {
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

inline uint32 ImageData::pixel(int x, int y) const {
  DCHECK(data_.get() != NULL);
  uint8* ptr = data_.get() + (y * width() + x) * sizeof_dataformat(format());
  return *(uint32*)ptr;
}

inline void ImageData::set_pixel(int x, int y, uint32 v) {
  uint8* ptr = data_.get() + (y * width() + x) * sizeof_dataformat(format());
  *(uint32*)ptr = v;
}

inline ImageData* ImageData::Load(const ::base::FilePath::StringType& path) {
  return Load(::base::FilePath(path));
}
}  // namespace azer
