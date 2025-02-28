#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Modifications, DataTypeNode) {

  dip::DIP d;
  d.add_string("foo int = 2"); 
  d.add_string("foo int32 = 3"); 
  dip::Environment env = d.parse();

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "2");
  EXPECT_EQ(node->dtype, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "3");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_INT32);
  
}
  
TEST(Modifications, ModificationNode) {

  dip::DIP d;
  d.add_string("foo int = 2"); 
  d.add_string("foo = 3"); 
  dip::Environment env = d.parse();

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "2");
  EXPECT_EQ(node->dtype, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "3");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_INT32);

  // in case modified node was not defined throw an exception
  d = dip::DIP();
  d.add_string("foo = 3"); 
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Modifications, Declarations) {

  dip::DIP d;
  d.add_string("foo int"); 
  d.add_string("foo = 3"); 
  dip::Environment env = d.parse();

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "");
  EXPECT_EQ(node->dtype, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->declared, true);

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "3");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_INT32);

  // if node is declared but has no value throw an exception
  d = dip::DIP();
  d.add_string("foo int"); 
  EXPECT_THROW(d.parse(), std::runtime_error);
 
}
