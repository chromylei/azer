#include "azer/animation/bone.h"

#include "azer/math/math.h"

namespace azer {
Bone* Bone::Find(const std::string& name) {
  Bone* bone = NULL;
  if (name == name_) return this;
  if (next_sibling_ && (bone = next_sibling_->Find(name))) {
    return bone;
  }

  if (first_child_ && (bone = first_child_->Find(name))) {
    return bone;
  }

  return NULL;
}

/**
 *
 */
void Bone::Transform(const Matrix4& mat) {
  /**
   * 骨骼动画的层次更新
   * 第一步完成自身层次的运动，通常是旋转，而这个旋转需要将本节骨折移动到原点处
   * 这就是 position_ 的作用，通过它可以产生一个 transform 到原点的矩阵
   * 第二部是将父节点的运行加持到当前节点处，而父节点完成对实际上与第一条是相同的
   * 第三部：将父节点的运动传递给兄弟节点
   * 第四部: 将自身运动及父节点运行结合起来传递给子节点
   *
   */
  if (next_sibling_) {
    Transform(mat);
  }

  if (first_child_) {
    Transform(mat * local_);
  }
}
}  // namespace azer
