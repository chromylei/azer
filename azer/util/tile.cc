#include "azer/util/tile.h"

#include <limits>
#include "azer/render/render.h"
#include "azer/render/axis_aligned_box.h"

namespace azer {
void Tile::Init() {
  min_x_ = std::numeric_limits<float>::max();
  max_x_ = -std::numeric_limits<float>::max();
  min_y_ = std::numeric_limits<float>::max();
  max_y_ = -std::numeric_limits<float>::max();
  min_z_ = std::numeric_limits<float>::max();
  max_z_ = -std::numeric_limits<float>::max();
  yspec_ = false;
  InitVertex();
  Pitch pitch;
  pitch.left = 0;
  pitch.top = 0;
  pitch.right = kGridLine - 1;
  pitch.bottom = kGridLine - 1;
  indices_.resize((kGridLine - 1) * (kGridLine - 1) * 6);
  InitPitchIndices(0, pitch, &(indices_[0]));
}

void Tile::InitVertex() {
  vertices_.clear();
  const int kVertexNum = kGridLine * kGridLine;
  vertices_.reserve(kVertexNum);
  for (int i = 0; i < kGridLine; ++i) {
    for (int j = 0; j < kGridLine; ++j) {
      int idx = i * kGridLine + j;
      float x = ((float)j - kGridLine / 2.0f) * kCellWidth;
      float z = ((float)i - kGridLine / 2.0f) * kCellWidth;
      vertices_.push_back(azer::vec3(x, 0.0f, z));
      if (x > max_x_) max_x_ = x;
      if (x < min_x_) min_x_ = x;
      if (z > max_z_) max_z_ = z;
      if (z < min_z_) min_z_ = z;
    }
  }
}

int32* Tile::InitPitchIndices(int level, const Tile::Pitch& pitch, int32* indices) {
  return azer::InitPitchIndices(level, pitch, kGridLine, indices);
}

void Tile::CalcNormal() {
  normal_.resize(GetVertexNum());
  std::vector<float> used(GetVertexNum(), 0.0f);
  for (int i = 0; i < GetIndicesNum(); i+=3) {
    int index1 = indices_[i];
    int index2 = indices_[i + 1];
    int index3 = indices_[i + 2];
    const Vector3& p1 = vertices_[index1];
    const Vector3& p2 = vertices_[index2];
    const Vector3& p3 = vertices_[index3];
    used[index1] += 1.0f;
    used[index2] += 1.0f;
    used[index3] += 1.0f;

    Vector3 normal = CalcPlaneNormal(p1, p2, p3);

    normal_[index1] += normal;
    normal_[index2] += normal;
    normal_[index3] += normal;
  }

  for (int i = 0; i < GetVertexNum(); ++i) {
    normal_[i] = normal_[i] / used[i];
  }
}

void Tile::SetHeight(int x, int z, float height) {
  vertices_[x * kGridLine + z].y = height;
  if (height < min_y_) min_y_ = height;
  if (height > max_y_) max_y_ = height;
  yspec_ = true;
}

void Tile::QuadTree::SplitPitch(Node* node) {
  DCHECK_GT(tail_, 0);
  DCHECK_GE(node->level, 0);
  Node* nodes = nodes_.get();
  Node& n1 = nodes[tail_ + 0];
  Node& n2 = nodes[tail_ + 1];
  Node& n3 = nodes[tail_ + 2];
  Node& n4 = nodes[tail_ + 3];
  node->children[0] = tail_;
  node->children[1] = tail_ + 1;
  node->children[2] = tail_ + 2;
  node->children[3] = tail_ + 3;
  node->splitted = true;
  tail_ += 4;

  
  int midx = node->midx();
  int midy = node->midy();
  n1.pitch.left = node->pitch.left;
  n1.pitch.right = midx;
  n1.pitch.top = node->pitch.top;
  n1.pitch.bottom = midy;
  n1.level = node->level - 1;
  n2.splitted = false;

  n2.pitch.left = midx;
  n2.pitch.right = node->pitch.right;
  n2.pitch.top = node->pitch.top;
  n2.pitch.bottom = midy;
  n2.level = node->level - 1;
  n2.splitted = false;

  n3.pitch.left = node->pitch.left;
  n3.pitch.right = midx;
  n3.pitch.top = midy;
  n3.pitch.bottom = node->pitch.bottom;
  n3.level = node->level - 1;
  n3.splitted = false;

  n4.pitch.left = midx;
  n4.pitch.right = node->pitch.right;
  n4.pitch.top = midy;
  n4.pitch.bottom = node->pitch.bottom;
  n4.level = node->level - 1;
  n4.splitted = false;
}

Tile::QuadTree::Splitable::SplitRes FrustrumSplit::Split(
    const Tile::QuadTree::Node& node) {
  const Vector3& minpos = tile_->vertex(node.pitch.left, node.pitch.top);
  const Vector3& maxpos = tile_->vertex(node.pitch.right,node.pitch.bottom);

  AxisAlignedBox box(Vector3(minpos.x, tile_->miny(), minpos.z),
                     Vector3(maxpos.x, tile_->maxy(), maxpos.z));
  VisibleState state = box.IsVisible(*frustrum_, Matrix4::kIdentity);
  if (state == kPartialVisible) {
    return kSplit;
  } else if (state == kFullyVisible) {
    return kKeep;
  }

  const Vector3& pos = frustrum_->camera()->position();
  if (pos.x >= minpos.x && pos.z >= minpos.z
      && pos.x <= maxpos.x && pos.z <= maxpos.z) {
    return kSplit;
  } else {
    return kDrop;
  }
}

void Tile::QuadTree::Split(int minlevel, Splitable* splitable,
                           std::vector<Tile::Pitch>* final) {
  DCHECK_GE(minlevel, 0);
  InitNode();
  int cur = 0;
  Node* node = nodes_.get();
  while (cur < tail_) {
    if (node->level == minlevel) {
      final->push_back(node->pitch);
    } else {
      DCHECK_GT(node->level, minlevel);
      int32 visible = splitable->Split(*node);
      if (visible == Splitable::kSplit) {
        SplitPitch(node);
      } else if (visible == Splitable::kKeep) {
        final->push_back(node->pitch);
      } else {
      }
    }
    cur++;
    node++;
  }
}

int32* InitPitchIndices(int level, const Tile::Pitch& pitch, int kGridLine,
                        int32* indices) {
  const int step = std::pow(2.0f, level);
  int32* cur = indices;
  for (int i = pitch.top; i < pitch.bottom; i += step) {
    for (int j = pitch.left; j < pitch.right; j += step) {
      int cur_line = i * kGridLine;
      int next_line = (i + step) * kGridLine;
      *cur++ = cur_line  + j;
      *cur++ = next_line + j;
      *cur++ = next_line + j + step;
      *cur++ = cur_line  + j;
      *cur++ = next_line + j + step;
      *cur++ = cur_line  + j + step;
    }
  }
  return cur;
}
}  // namespace util
