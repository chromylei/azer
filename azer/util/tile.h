#pragma once

#include <string>
#include <vector>
#include <cmath>

#include "base/files/file_path.h"
#include "azer/render/render.h"

namespace azer {
namespace util {
class Tile {
 public:
  Tile(const int level = 8)
      : kCellNum_(std::pow(2.0f, level) + 1)
      , kLevel_(level) {
  }

  void Init();
  const std::vector<azer::Vector3>& vertices() { return vertices_;}
  const std::vector<azer::Vector3>& normal() { return normal_;}
  const std::vector<int32>& indices() { return indices_;}
  void SetHeight(int x, int z, float height);

  int GetCellNum() const { return kCellNum_;}
  int GetVertexNum() const { return vertices_.size();}
  int GetIndicesNum() const { return indices_.size();}  
  void CalcNormal();

  float minx() const;
  float maxx() const;
  float minz() const;
  float maxz() const;
  float x_range() const { return maxx() - minx();}
  float z_range() const { return maxz() - minz();}
 private:
  void InitVertex();
  std::vector<azer::Vector3> vertices_;
  std::vector<azer::Vector3> normal_;
  std::vector<int32> indices_;
  const int kCellNum_;
  const int kLevel_;
  float min_x_, max_x_, min_z_, max_z_;
  DISALLOW_COPY_AND_ASSIGN(Tile);
};
}  // namespace util
}  // namespace azer
