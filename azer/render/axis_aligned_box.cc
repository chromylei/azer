#include "azer/render/axis_aligned_box.h"

#include <limits>

#include "azer/render/render_system.h"
#include "azer/render/mesh.h"
#include "azer/render/frustrum.h"
#include "azer/render/camera.h"
#include "azer/render/util/axis_aligned_box_mesh.h"
#include "base/logging.h"

namespace azer {

AxisAlignedBox AxisAlignedBox::CalcForVertexData(
    const VertexData& vdata) {
  azer::Vector4 min_pos(std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(), 1.0f);
  azer::Vector4 max_pos(std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min(), 1.0f);
  DataFormat pos_type = vdata.desc()->descs()[0].type;
  DCHECK(pos_type == kVec3 || pos_type == kVec4);
  uint8* next = vdata.pointer();
  CHECK(next != NULL);
  do {
    if (pos_type == kVec3) {
      azer::Vector3* pos = (azer::Vector3*)next;
      min_pos.x = std::min(min_pos.x, pos->x);
      min_pos.y = std::min(min_pos.y, pos->y);
      min_pos.z = std::min(min_pos.z, pos->z);

      max_pos.x = std::max(max_pos.x, pos->x);
      max_pos.y = std::max(max_pos.y, pos->y);
      max_pos.z = std::max(max_pos.z, pos->z);
    } else if (pos_type == kVec4) {
      azer::Vector4* pos = (azer::Vector4*)next;
      min_pos.x = std::min(min_pos.x, pos->x);
      min_pos.y = std::min(min_pos.y, pos->y);
      min_pos.z = std::min(min_pos.z, pos->z);
      
      max_pos.x = std::max(max_pos.x, pos->x);
      max_pos.y = std::max(max_pos.y, pos->y);
      max_pos.z = std::max(max_pos.z, pos->z);
    } else {
      CHECK(false) << "Type not support: " << pos_type;
    }
  } while ((next = vdata.next(next)));

  return AxisAlignedBox(min_pos, max_pos);
}

/*
AxisAlignedBox AxisAlignedBox::CalcForMesh(const Mesh& mesh) {
  azer::Vector4 min_pos(std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(), 1.0f);
  azer::Vector4 max_pos(std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min(), 1.0f);
  AxisAlignedBox ret;
  ret.minimum_ = min_pos;
  ret.maximum_ = max_pos;
  for (auto iter = mesh.Subs().begin(); iter != mesh.Subs().end(); ++iter) {
    VertexData* vdata = (*iter)->vb_ptr->data().get();
    DCHECK(NULL != vdata);
    AxisAlignedBox aabb = CalcForVertexData(*vdata);
    ret.merge(aabb);
  }

  return ret;
}
*/

AxisAlignedBox AxisAlignedBox::TransformCopy(const Matrix4& trans) {
  AxisAlignedBox box;
  box.minimum_ = trans * minimum_;
  box.maximum_ = trans * maximum_;
  return box;
}

bool AxisAlignedBox::intersect(const Ray& ray) const {
  // front
  // just test a plane inside the cube
  TrianglePlane plane1(minimum_, maximum_, vec3(maximum_.x, minimum_.y, maximum_.z));
  TrianglePlane plane2(minimum_, maximum_, vec3(minimum_.x, maximum_.y, minimum_.z));

  vec3 pos = plane1.intersect(ray);
  // LOG(ERROR) << "interact: " << pos << ", dist: " << plane1.distance(ray);
  // LOG(ERROR) << *this;
  return this->inside(pos);
}

namespace {
inline VisibleState IsAABBVisible(const AxisAlignedBox& aabb,
                                  const Frustrum& frustrum) {
  Vector3 center((aabb.maximum() + aabb.minimum()) / 2.0f);
  Vector3 halfsize((aabb.maximum() - aabb.minimum()) / 2.0f);
  return frustrum.IsVisible(center, halfsize);
}
}  // namespace

VisibleState AxisAlignedBox::IsVisible(const Frustrum& frustrum,
                                       const Matrix4& world) {
  AxisAlignedBox aabb = std::move(TransformCopy(world));
  return IsAABBVisible(aabb, frustrum);
}

void AxisAlignedBox::Render(Renderer* renderer, const Matrix4& world,
                                    const Camera& camera) {
  Vector3 scale = maximum() - minimum();
  azer::Matrix4 new_world = std::move(world * azer::Scale(scale));
  azer::Matrix4 wvp = std::move(camera.GetProjViewMatrix() * new_world);
  AABBMesh* mesh = AABBMesh::GetInstance(renderer->GetRenderSystem());
  mesh->effect()->SetWVP(wvp);
  mesh->effect()->SetDiffuse(azer::Vector4(0.0f, 0.6f, 0.0f, 1.0f));
  mesh->effect()->Use(renderer);
  mesh->Render(renderer);
}

std::ostream& operator << (std::ostream& o, const AxisAlignedBox& v) {
  o << "AxisAlignedBox["
    << " minimum: (" << v.minimum_.x << ", " << v.minimum_.y << ", "
    << v.minimum_.z << ") "
    << ", maximum: (" << v.maximum_.x << ", " << v.maximum_.y << ", " << v.maximum_.z << ") ]";
  return o;
}
}  // namespace azer
