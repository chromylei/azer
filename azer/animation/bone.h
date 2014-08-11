#pragma once

#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/render_export.h"

namespace azer {
class AZER_EXPORT Bone {
 public:
  Bone();

  Bone* Find(const std::string& name);

  void Transform(const Matrix4& mat);
 private:
  Bone* first_child_;
  Bone* next_sibling_;

  Matrix4 inverse_;
  Matrix4 world_;
  Matrix4 rotate_;
  Matrix4 local_;

  azer::Vector3 position_;
  std::string name_;
  DISALLOW_COPY_AND_ASSIGN(Bone);
};
}  // namespace azer
