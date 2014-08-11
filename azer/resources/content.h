#pragma once

#include <utility>
#include "azer/base/render_export.h"

namespace azer {

struct AZER_EXPORT FileContent {
  int64 length;
  char data[1];
};

typedef std::shared_ptr<FileContent> FileContentPtr;
}  // namespace azer
