#include "azer/util/tile.h"

namespace azer {
namespace util {

void Tile::Init() {
  InitVertex();
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

}  // namespace azer {
}  // namespace util
