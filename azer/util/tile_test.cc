#include "azer/util/tile.cc"
#include "gtest/gtest.h"

using azer::util::Tile;
using azer::util::QuadTree;

TEST(Tile, Indices) {
  int level = 8;
  Tile tile(level);
  tile.Init();
  const int kGridLine = (std::pow(2.0, level) + 1);
  ASSERT_EQ(tile.vertices().size(), kGridLine * kGridLine);
  ASSERT_EQ(tile.indices().size(), (kGridLine - 1) * (kGridLine - 1) * 2 * 3);
}

TEST(Tile, PitchIndices) {
}

namespace {
class TileQuadTreeBase : public QuadTree::Splitable {
 public:
  virtual SplitRes Split(const QuadTree::Node& node) {
    if (node.level > 0) {
      return QuadTree::Splitable::kSplit;
    } else {
      return QuadTree::Splitable::kKeep;
    }
  }
};
}

TEST(TileQuadTree, Base) {
  int level = 2;
  Tile tile(level);
  tile.Init();

  TileQuadTreeBase basequad;
  QuadTree quad(level);
  std::vector<Tile::Pitch> nodes;
  quad.Split(level - 1, &basequad, &nodes);
  ASSERT_EQ(nodes.size(), 4u);
}
