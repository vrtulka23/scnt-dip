#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Expressions, Logical) {

  // define scalar
  dip::DIP d;
  d.add_string("foo bool = ('true && false')");
  d.add_string("bar bool = ('true || false')");
  d.add_string("snap bool = true");
  d.add_string("crackle bool = ('{?snap} || false')");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 4);
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->name, "foo");
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "false");

  node = env.nodes.at(1);
  EXPECT_EQ(node->name, "bar");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "true");

  node = env.nodes.at(3);
  EXPECT_EQ(node->name, "crackle");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "true");

}
