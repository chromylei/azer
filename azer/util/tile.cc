#include "azer/util/tile.h"

#include <limits>
#include "base/logging.h"

namespace azer {
namespace util {

void Tile::Init() {
  min_x_ = std::numeric_limits<float>::max();
  max_x_ = -std::numeric_limits<float>::max();
  min_z_ = std::numeric_limits<float>::max();
  max_z_ = -std::numeric_limits<float>::max();
  InitVertex();
}

float Tile::minx() const {
  DCHECK_GT(vertices_.size(), 0u);
  return min_x_;
}
float Tile::maxx() const {
  DCHECK_GT(vertices_.size(), 0u);
  return max_x_;
}
float Tile::minz() const {
  DCHECK_GT(vertices_.size(), 0u);
  return min_z_;
}
float Tile::maxz() const {
  DCHECK_GT(vertices_.size(), 0u);
  return max_z_;
}

void Tile::InitVertex() {
  const int kRow = kCellNum_;
  const int kColumn = kCellNum_;
  const int kVertexNum = kRow * kColumn;
  for (int i = 0; i < kColumn; ++i) {
    for (int j = 0; j < kRow; ++j) {
      int idx = i * kRow + j;
      float x = (float)j - kRow / 2.0f;
      float z = (float)i - kColumn / 2.0f;
      vertices_.push_back(azer::vec3(x, 0.0f, z));
      if (x > max_x_) max_x_ = x;
      if (x < min_x_) min_x_ = x;
      if (z > max_z_) max_z_ = z;
      if (z < min_z_) min_z_ = z;
    }
  }

  const int kTriangleNum = (kRow - 1) * (kColumn - 1) * 2;
  for (int i = 0; i < kColumn-1; ++i) {
    for (int j = 0; j < kRow-1; ++j) {
      indices_.push_back(i * kRow + j);
      indices_.push_back((i + 1) * kRow + j + 1);
      indices_.push_back((i + 1) * kRow + j);
      indices_.push_back(i * kRow + j);
      indices_.push_back(i * kRow + j + 1);
      indices_.push_back((i + 1) * kRow + j + 1);
    }
  }
}

void Tile::CalcNormal() {
  normal_.resize(GetVertexNum());
  std::vector<uint32> used(GetVertexNum(), 0);
  for (int i = 0; i < GetIndicesNum(); i+=3) {
    int index1 = indices_[i];
    int index2 = indices_[i + 1];
    int index3 = indices_[i + 2];
    const Vector3& p1 = vertices_[index1];
    const Vector3& p2 = vertices_[indices_[index2]];
    const Vector3& p3 = vertices_[indices_[index3]];
    used[indices_[index1]]++;
    used[indices_[index2]]++;
    used[indices_[index3]]++;
    Vector3 normal = CalcPlaneNormal(p1, p2, p3);

    normal_[index1] += normal;
    normal_[index2] += normal;
    normal_[index3] += normal;
  }

  for (int i = 0; i < GetVertexNum(); ++i) {
    normal_[i] /= (float)used[i];
  }
}

void Tile::SetHeight(int x, int z, float height) {
  vertices_[x * kCellNum_ + z].y = height;
}
}  // namespace azer {
}  // namespace util
