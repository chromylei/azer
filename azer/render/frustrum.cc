#include "azer/render/frustrum.h"

#include "azer/render/camera.h"
#include "base/logging.h"

namespace azer {
void Frustrum::UpdatePlane() {
  DCHECK(NULL != camera_);
  const Matrix4& combo = camera_->GetProjViewMatrix();
  
  planes_[kNearPlane].normal().x = combo[3][0] + combo[2][0];
  planes_[kNearPlane].normal().y = combo[3][1] + combo[2][1];
  planes_[kNearPlane].normal().z = combo[3][2] + combo[2][2];
  planes_[kNearPlane].d()        = combo[3][3] + combo[2][3];

  planes_[kFarPlane].normal().x = combo[3][0] - combo[2][0];
  planes_[kFarPlane].normal().y = combo[3][1] - combo[2][1];
  planes_[kFarPlane].normal().z = combo[3][2] - combo[2][2];
  planes_[kFarPlane].d()        = combo[3][3] - combo[2][3];

  planes_[kLeftPlane].normal().x = combo[3][0] + combo[0][0];
  planes_[kLeftPlane].normal().y = combo[3][1] + combo[0][1];
  planes_[kLeftPlane].normal().z = combo[3][2] + combo[0][2];
  planes_[kLeftPlane].d()        = combo[3][3] + combo[0][3];

  planes_[kRightPlane].normal().x = combo[3][0] - combo[0][0];
  planes_[kRightPlane].normal().y = combo[3][1] - combo[0][1];
  planes_[kRightPlane].normal().z = combo[3][2] - combo[0][2];
  planes_[kRightPlane].d()        = combo[3][3] - combo[0][3];

  planes_[kTopPlane].normal().x = combo[3][0] - combo[1][0];
  planes_[kTopPlane].normal().y = combo[3][1] - combo[1][1];
  planes_[kTopPlane].normal().z = combo[3][2] - combo[1][2];
  planes_[kTopPlane].d()        = combo[3][3] - combo[1][3];

  planes_[kBottomPlane].normal().x = combo[3][0] + combo[1][0];
  planes_[kBottomPlane].normal().y = combo[3][1] + combo[1][1];
  planes_[kBottomPlane].normal().z = combo[3][2] + combo[1][2];
  planes_[kBottomPlane].d()        = combo[3][3] + combo[1][3];

  for (int i = 0; i < 6; ++i) {
    planes_[i].Normalize();
  }
}


bool Frustrum::IsVisible(const Vector3& point) const {
  for (int i = 0; i < 6; ++i) {
    if (planes_[i].GetSide(point) == Plane::kNegative) {
      return false;
    }
  }

  return true;
}

bool Frustrum::IsVisible(const Vector3& center,
                         const Vector3& halfsize) const {
  for (int i = 0; i < 6; ++i) {
    if (planes_[i].GetSide(center, halfsize) == Plane::kNegative) {
      return false;
    }
  }

  return true;
}
}  // namespace azer
