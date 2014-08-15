#pragma once

#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "azer/render/render.h"

namespace azer {
namespace util {
class Tile {
 public:
  Tile(const int cell = 1024) : kCellNum_(cell) {}

  void Init();
  const std::vector<azer::Vector3>& vertices() { return vertices_;}
  const std::vector<int32>& indices() { return indices_;}

  int GetCellNum() const { return kCellNum_;}
 private:
  void InitVertex();
  std::vector<azer::Vector3> vertices_;
  std::vector<int32> indices_;
  const int kCellNum_;
  DISALLOW_COPY_AND_ASSIGN(Tile);
};

}  // namespace util
}  // namespace azer
