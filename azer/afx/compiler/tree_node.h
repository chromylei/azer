#pragma once

#include "azer/base/tree_node.h"

namespace azer {
namespace afx {

/* class TreeNodeExtend
 * 一般情况下，都是通过 parent 创建，但在一些 down-up 的应用中，父节点最后才能确定
 *
 */
template <class T>
class TreeNodeDownUp : public TreeNode<T> {
 public:
  void AddSibling(T* sibling) {
    DCHECK(sibling != this);
    T* cur = (T*)this;
    T* prev = cur;
    while (cur) {
      prev = cur;
      cur = cur->next_sibling();
    }
    prev->next_sibling_ = sibling;
    sibling->prev_sibling_ = prev;
  }

  void AddChildren(T* sibling) {
    T* cur = sibling;
    while (cur) {
      cur->reset();
      AddChild(cur);
      T* n = cur->next_sibling();
      cur = n;
    }
  }

  /**
   * 使用节点 node 替换掉 this 节点的位置
   */
  void ReplaceBy(T* node) {
    node->prev_sibling_ = prev_sibling();
    node->next_sibling_ = next_sibling();
    node->first_child_ = first_child();
    node->last_child_ = last_child();
    node->parent_ = parent();
    if (parent()->last_child() == this) {
      parent()->last_child_ = node;
    }

    if (parent()->first_child() == this) {
      parent()->first_child_ = node;
    }
    reset();
  }

  void Replace(T* node) {
    node->ReplaceBy((T*)this);
  }
};

}  // namespace afx
}  // namespace azer

