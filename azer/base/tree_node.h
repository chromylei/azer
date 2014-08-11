#pragma once

#include <string>

#include "base/logging.h"
#include "base/basictypes.h"
#include "azer/base/string.h"
#include "azer/base/render_export.h"

namespace azer {

template<class T>
class TreeNode {
 public:
  TreeNode(const StringType& name, T* parent)
      : node_name_(name)
      , parent_(parent)
      , first_child_(NULL)
      , last_child_(NULL)
      , next_sibling_(NULL)
      , prev_sibling_(NULL) {
  }

  TreeNode()
      : parent_(NULL)
      , first_child_(NULL)
      , last_child_(NULL)
      , next_sibling_(NULL)
      , prev_sibling_(NULL) {
  }

  /**
   * 遍历 TreeNode 组成的树，此遍历接口可以同时支持先序遍历和后序遍历两种方式
   * 如果希望后续遍历树, 只要相应 OnVisitEnd 即可
   */
  class Traverser {
   public:
    virtual ~Traverser() {}
    virtual bool OnVisitBegin(T* node) = 0;
    virtual void OnVisitEnd(T* node) = 0;
  };

  void traverse(Traverser* delegate);

  const StringType& node_name() const { return node_name_;}
  T* find(const StringType& name);

  bool IsRoot() const { return parent_ == NULL;}
  bool HasChildren() const { return first_child_ != NULL;}
  void AddChild(T* node);
  void RemoveChild(T* node);
  void InsertBefore(T *before);
  void InsertAfter(T *after);
  T* parent() { return parent_;}
  const T* parent() const { return parent_;}

  T* first_child() { return first_child_;}
  const T* first_child() const { return first_child_;}
  T* last_child() { return last_child_;}
  const T* last_child() const { return last_child_;}

  T* prev_sibling() { return prev_sibling_;}
  const T* prev_sibling() const { return prev_sibling_;}
  T* next_sibling() { return next_sibling_;}
  const T* next_sibling() const { return next_sibling_;}

  T* pointer() { return (T*)this;}

  void reset();
  void reset_sibling();
 protected:
  StringType node_name_;
  T* parent_;
  T* first_child_;
  T* last_child_;
  T* next_sibling_;
  T* prev_sibling_;
 private:
  DISALLOW_COPY_AND_ASSIGN(TreeNode);
};

template<class T>
void TreeNode<T>::AddChild(T* node) {
  DCHECK(node->next_sibling_ == NULL);
  DCHECK(node->prev_sibling_ == NULL);
  if (last_child()) {
    DCHECK(last_child_->next_sibling_ == NULL);
    last_child_->next_sibling_ = node;
    node->prev_sibling_ = last_child_;
  }

  if (!first_child_) {
    first_child_ = node;
  }

  node->parent_ = (T*)this;
  last_child_ = node;
}

template<class T>
void TreeNode<T>::RemoveChild(T* node) {
  DCHECK(node->parent_ == this);
  if (node->prev_sibling_ == NULL) {
    DCHECK(first_child_ == node);
    first_child_ = node->next_sibling_;
  }

  if (node == last_child_) {
    last_child_ = node->prev_sibling_;
  }

  if (node->prev_sibling_) {
    node->prev_sibling_->next_sibling_ = node->next_sibling_;
  }

  if (node->next_sibling_) {
    node->next_sibling_->prev_sibling_ = node->prev_sibling_;
  }

  node->reset();
}

template<class T>
void TreeNode<T>::reset() {
  node_name_.clear();
  parent_ = NULL;
  first_child_  = NULL;
  last_child_ = NULL;
  reset_sibling();
}

template<class T>
void TreeNode<T>::reset_sibling() {
  next_sibling_ = NULL;
  prev_sibling_ = NULL;
}

template<class T>
T* TreeNode<T>::find(const StringType& name) {
  if (node_name_ == name) return (T*)this;
  T* node = first_child();
  for (; node != NULL; node = node->next_sibling()) {
    T* ret = node->find(name);
    if (ret) return ret;
  }

  return NULL;
}

template<class T>
void TreeNode<T>::InsertBefore(T *before) {
  this->parent_ = before->parent();
  this->next_sibling_ = before;
  if (before->prev_sibling()) {
    before->prev_sibling_->next_sibling_ = (T*)this;
    this->prev_sibling_ = before->prev_sibling();
    before->prev_sibling_ = (T*)this;
  } else {
    this->prev_sibling_ = NULL;
    before->prev_sibling_ = (T*)this;
    before->parent_->first_child_ = (T*)this;
  }
}

template<class T>
void TreeNode<T>::InsertAfter(T *after) {
  this->parent_ = after->parent();
  this->prev_sibling_ = after;
  if (after->next_sibling()) {
    this->next_sibling_ = after->next_sibling_;
    this->next_sibling_->prev_sibling_ = (T*)this;
    after->next_sibling_ = (T*)this;
  } else {
    after->next_sibling_ = (T*)this;
    this->next_sibling_ = NULL;
    this->parent_->last_child_ = (T*)this;
  }
}

template<class T>
void TreeNode<T>::traverse(Traverser* traverse) {
  if (!traverse->OnVisitBegin((T*)this)) return;

  T* node = first_child();
  while (node) {
    DCHECK(node->parent() == this);
    node->traverse(traverse);
    node = node->next_sibling();
  }

  traverse->OnVisitEnd((T*)this);
}
}  // namespace azer
