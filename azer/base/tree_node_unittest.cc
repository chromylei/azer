#include "azer/base/tree_node.h"
#include "gtest/gtest.h"

using azer::StringType;

namespace {
class SceneNode : public azer::TreeNode<SceneNode> {
 public:
  SceneNode(const StringType& name) : azer::TreeNode<SceneNode>(name, NULL) {}
 private:
  StringType name_;
};
}  // namespace

TEST(TreeNode, Root) {
  SceneNode root(AZER_LITERAL("root"));
  ASSERT_TRUE(root.IsRoot());
  ASSERT_TRUE(root.parent() == NULL);
  ASSERT_FALSE(root.HasChildren());
  ASSERT_TRUE(root.first_child() == NULL);
  ASSERT_TRUE(root.last_child() == NULL);
}

TEST(TreeNode, InsertAfter) {
  SceneNode root(AZER_LITERAL("root"));
  SceneNode level_1_1(AZER_LITERAL("level1_1"));
  SceneNode level_1_2(AZER_LITERAL("level1_2"));
  SceneNode level_1_3(AZER_LITERAL("level1_3"));
  root.AddChild(&level_1_2);
  ASSERT_TRUE(level_1_2.parent() == &root);
  ASSERT_TRUE(level_1_2.prev_sibling() == NULL);
  ASSERT_TRUE(level_1_2.next_sibling() == NULL);
  ASSERT_TRUE(root.first_child() == &level_1_2);
  ASSERT_TRUE(root.last_child() == &level_1_2);

  level_1_1.InsertBefore(&level_1_2);
  ASSERT_TRUE(level_1_1.parent() == &root);
  ASSERT_TRUE(level_1_1.prev_sibling() == NULL);
  ASSERT_TRUE(level_1_1.next_sibling() == &level_1_2);
  ASSERT_TRUE(level_1_2.prev_sibling() == &level_1_1);
  ASSERT_TRUE(level_1_2.next_sibling() == NULL);
  ASSERT_TRUE(root.first_child() == &level_1_1);
  ASSERT_TRUE(root.last_child() == &level_1_2);

  level_1_3.InsertAfter(&level_1_2);
  ASSERT_TRUE(level_1_3.parent() == &root);
  ASSERT_TRUE(level_1_3.prev_sibling() == &level_1_2);
  ASSERT_TRUE(level_1_3.next_sibling() == NULL);
  ASSERT_TRUE(level_1_2.prev_sibling() == &level_1_1);
  ASSERT_TRUE(level_1_2.next_sibling() == &level_1_3);
  ASSERT_TRUE(root.first_child() == &level_1_1);
  ASSERT_TRUE(root.last_child() == &level_1_3);
}


TEST(TreeNode, AddAndRemoveChild) {
  SceneNode root(AZER_LITERAL("root"));
  SceneNode level_1_1(AZER_LITERAL("level1_1"));
  SceneNode level_1_2(AZER_LITERAL("level1_2"));
  SceneNode level_1_3(AZER_LITERAL("level1_3"));
  SceneNode level_2_1(AZER_LITERAL("level2_1"));
  SceneNode level_2_2(AZER_LITERAL("level2_2"));
  SceneNode level_2_3(AZER_LITERAL("level2_3"));
  root.AddChild(&level_1_1);
  root.RemoveChild(&level_1_1);
  root.AddChild(&level_1_1);
  ASSERT_TRUE(level_1_1.parent() == &root);
  ASSERT_TRUE(level_1_1.prev_sibling() == NULL);
  ASSERT_TRUE(level_1_1.next_sibling() == NULL);
  ASSERT_TRUE(root.first_child() == &level_1_1);
  ASSERT_TRUE(root.last_child() == &level_1_1);

  root.AddChild(&level_1_2);
  root.RemoveChild(&level_1_2);
  root.AddChild(&level_1_2);
  ASSERT_TRUE(level_1_1.parent() == &root);
  ASSERT_TRUE(level_1_2.parent() == &root);
  ASSERT_TRUE(level_1_1.prev_sibling() == NULL);
  ASSERT_TRUE(level_1_1.next_sibling() == &level_1_2);
  ASSERT_TRUE(level_1_2.prev_sibling() == &level_1_1);
  ASSERT_TRUE(level_1_2.next_sibling() == NULL);
  ASSERT_TRUE(root.first_child() == &level_1_1);
  ASSERT_TRUE(root.last_child() == &level_1_2);

  root.AddChild(&level_1_3);
  root.RemoveChild(&level_1_3);
  root.AddChild(&level_1_3);
  ASSERT_TRUE(level_1_1.parent() == &root);
  ASSERT_TRUE(level_1_2.parent() == &root);
  ASSERT_TRUE(level_1_3.parent() == &root);  
  ASSERT_TRUE(level_1_1.prev_sibling() == NULL);
  ASSERT_TRUE(level_1_1.next_sibling() == &level_1_2);
  ASSERT_TRUE(level_1_2.prev_sibling() == &level_1_1);
  ASSERT_TRUE(level_1_2.next_sibling() == &level_1_3);
  ASSERT_TRUE(level_1_3.prev_sibling() == &level_1_2);
  ASSERT_TRUE(level_1_3.next_sibling() == NULL);
  ASSERT_TRUE(root.first_child() == &level_1_1);
  ASSERT_TRUE(root.last_child() == &level_1_3);
}
