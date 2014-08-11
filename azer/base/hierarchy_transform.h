#pragma once

#include "azer/base/tree_node.h"
#include "base/basictypes.h"
#include "azer/math/math.h"
#include "azer/base/movable.h"
#include "azer/base/render_export.h"

namespace azer {
template <class T>
class HierarchyTransform : public TreeNode<T>
                         , public MovableObject {
 public:
  HierarchyTransform(const typename StringType& name, T* parent,
                     const Vector3& pos, const Vector3& scale,
                     const Quaternion& orient)
      : MovableObject(pos, orient)
      , TreeNode(name, parent)
      , scale_(scale) {
  }

  void reset();
  // void Scale(const Vector3& scale);
  void Translate(const Vector3& pos);
  void Rotation(const Matrix4& rotate);
  void Rotation(const Quaternion& quaternion);

  const Matrix4& GetWorldMatrix() const { return world_;}
 protected:
  void UpdateHierarchy() {
    Traverser<T> traverser;
    this->traverse(&traverser);
  }
 private:
  void UpdateWorldMatrix();

  template <class T2>
  class Traverser : public TreeNode<T2>::Traverser {
   public:
    // member of traverser
    virtual bool OnVisitBegin(T2* node) OVERRIDE {
      node->UpdateWorldMatrix(); return true;
    }
    virtual void OnVisitEnd(T2* node) OVERRIDE {}
  };
  
  Vector3 scale_;
  Matrix4 world_;
  DISALLOW_COPY_AND_ASSIGN(HierarchyTransform);
};

template <class T>
void HierarchyTransform<T>::reset() {
  world_ = Matrix4::kIdentity;
}

template <class T>
void HierarchyTransform<T>::Translate(const Vector3& pos) {
  TreeNode<T>* node = first_child();
  for (; node != NULL; node = node->next_sibling()) {
    node->pointer()->Translate(pos);
  }  
}

template <class T>
void HierarchyTransform<T>::Rotation(const Matrix4& rotate) {
}

template <class T>
void HierarchyTransform<T>::Rotation(const Quaternion& quaternion) {
}

template <class T>
void HierarchyTransform<T>::UpdateWorldMatrix() {
  world_ = std::move(orientation().ToMatrix() * azer::Scale(scale_));
  world_ = std::move(azer::Translate(position()) * world_);
  if (parent()) {
    world_ = std::move(parent()->GetWorldMatrix() * world_);
  }
}

}  // namespace azer
