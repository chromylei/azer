#pragma once

#include <string>
#include <vector>
#include <queue>
#include <cmath>

#include "base/files/file_path.h"
#include "base/logging.h"
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
  float x_range() const { return maxx() - minx();}

  float miny() const;
  float maxy() const;
  float y_range() const { return maxy() - miny();}
  
  float minz() const;
  float maxz() const;
  float z_range() const { return maxz() - minz();}
 private:
  void InitVertex();
  std::vector<azer::Vector3> vertices_;
  std::vector<azer::Vector3> normal_;
  std::vector<int32> indices_;
  const int kCellNum_;
  const int kLevel_;
  float min_x_, max_x_, min_y_, max_y_, min_z_, max_z_;
  bool yspec_;
  DISALLOW_COPY_AND_ASSIGN(Tile);
};

class QuadTree {
 public:
  struct Pitch {
    int left;
    int right;
    int top;
    int bottom;

    Pitch(): left(-1), right(-1), top(-1), bottom(-1) {}
    Pitch(const Pitch& pitch) {*this = pitch; }
    Pitch& operator = (const Pitch& pitch);
  };

  struct Node {
    Pitch pitch;
    int children[4];
    int level;
    bool splitted;
    Node(): level(-1), splitted(false) {}
    int midx() const { return std::pow(2.0, level - 1) +  pitch.left;}
    int midy() const { return std::pow(2.0, level - 1) +  pitch.top;}
  };

  explicit QuadTree(int level)
      : tail_(-1)
      , kLevel(level)
      , kCellNum(std::pow(2.0, level) + 1) {
    int max_cell = (int)std::pow(4.0, level) / 3;
    nodes_.reset(new Node[max_cell]);
  }

  class Splitable {
   public:
    virtual bool Split(const Node& node) = 0;
  };
  void Split(Splitable* splitable, std::deque<Node*>* nodes);
 private:
  void InitNode();
  void SplitPitch(Node* node);
  int tail_;
  std::unique_ptr<Node[]> nodes_;
  const int kLevel;
  const int kCellNum;
  DISALLOW_COPY_AND_ASSIGN(QuadTree);
};

class FrustrumSplit : public QuadTree::Splitable {
 public:
  FrustrumSplit(Tile* tile) : tile_(tile) {}
  virtual bool Split(const QuadTree::Node& node) OVERRIDE;
 private:
  Tile* tile_;
  DISALLOW_COPY_AND_ASSIGN(FrustrumSplit);
};

inline void QuadTree::InitNode() {
  tail_ = 0;
  Node& n = (nodes_.get())[0];
  n.splitted = false;
  n.pitch.left = 0;
  n.pitch.top = 0;
  n.pitch.right = kCellNum;
  n.pitch.bottom = kCellNum;
}

inline void QuadTree::Split(Splitable* splitable, std::deque<Node*>* final) {
  InitNode();
  int cur = tail_;
  Node* node = nodes_.get();
  while (cur <= tail_) {
    if (!splitable->Split(*node)) {
      final->push_back(node);
    }
    cur++;
    node++;
  }
}

inline QuadTree::Pitch& QuadTree::Pitch::operator = (const Pitch& pitch) {
  left = pitch.left;
  right = pitch.right;
  top = pitch.top;
  bottom = pitch.bottom;
  return *this;
}

inline float Tile::minx() const {
  DCHECK_GT(vertices_.size(), 0u);
  return min_x_;
}
inline float Tile::maxx() const {
  DCHECK_GT(vertices_.size(), 0u);
  return max_x_;
}
inline float Tile::miny() const {
  DCHECK(yspec_);
  DCHECK_GT(vertices_.size(), 0u);
  return min_y_;
}
inline float Tile::maxy() const {
  DCHECK(yspec_);
  DCHECK_GT(vertices_.size(), 0u);
  return max_y_;
}
inline float Tile::minz() const {
  DCHECK_GT(vertices_.size(), 0u);
  return min_z_;
}
inline float Tile::maxz() const {
  DCHECK_GT(vertices_.size(), 0u);
  return max_z_;
}
}  // namespace util
}  // namespace azer
