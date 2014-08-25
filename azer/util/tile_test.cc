#include "azer/util/tile.cc"
#include "gtest/gtest.h"

using azer::util::Tile;
TEST(Tile, Indices) {
  int level = 8;
  Tile tile(level);
  tile.Init();
  int edge = (std::pow(2.0, level) + 1);
  ASSERT_EQ(tile.vertices().size(), edge * edge);
  ASSERT_EQ(tile.indices().size(), (edge - 1) * (edge - 1) * 2 * 3);
}

TEST(Tile, PitchIndices) {
}
