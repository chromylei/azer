#pragma once

#include "azer/base/render_export.h"

namespace azer {
class Frustrum;

class AZER_EXPORT Cullable {
 public:
  virtual bool IsVisible(const Frustrum& frustrum) = 0;
};
}  // namespace azer
