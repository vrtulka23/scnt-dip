#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(ParseScalars, BooleanValue) {
  
  dip::DIP d;    
  d.add_string("foo1 bool = true");
  d.add_string("foo2 bool = false");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "true");
  EXPECT_EQ(node->dtype, dip::Node::NODE_BOOLEAN);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "true");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_BOOL);
	
  node = env.nodes[1];
  EXPECT_EQ(node->value_raw, "false");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_EQ(vnode->value->to_string(), "false");

}

TEST(ParseScalars, IntegerValue) {

  dip::DIP d;
  d.add_string("foo1 int = 23");
  d.add_string("foo2 int = 23456789");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "23");
  EXPECT_EQ(node->dtype, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "23");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_INT32);
  
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_EQ(vnode->value->to_string(), "23456789");
  
}

TEST(ParseScalars, FloatValue) {

  dip::DIP d;
  d.add_string("foo1 float = 23");
  d.add_string("foo2 float = 23.456");
  d.add_string("foo3 float = 23.456e7");
  d.add_string("foo4 float32 = 23.123456789123456789");
  d.add_string("foo5 float64 = 23.123456789123456789");
  if (dip::FloatNode::max_float_size==128) 
    d.add_string("foo6 float128 = 23.123456789123456789");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "23");
  EXPECT_EQ(node->dtype, dip::Node::NODE_FLOAT);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "23.000");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_FLOAT64);
  
  node = env.nodes[1];
  EXPECT_EQ(node->value_raw, "23.456");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_EQ(vnode->value->to_string(), "23.456");
  
  node = env.nodes[2];
  EXPECT_EQ(node->value_raw, "23.456e7");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[2]);
  EXPECT_EQ(vnode->value->to_string(), "2.3456e+08");

  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[3]);
  EXPECT_EQ(vnode->value->to_string(7), "23.123457");

  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[4]);
  EXPECT_EQ(vnode->value->to_string(13), "23.123456789123");

  // TODO: This case needs more testing on other platforms
  if (dip::FloatNode::max_float_size==128) {
    vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[5]);
    std::cout << dip::FloatNode::max_float_size << std::endl;
    EXPECT_EQ(vnode->value->to_string(18), "23.123456789123456789");
  }

}

TEST(ParseScalars, StringValue) {
  
  dip::DIP d;    
  d.add_string("foo str = \"bar\"");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "bar");
  EXPECT_EQ(node->dtype, dip::Node::NODE_STRING);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "bar");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::VALUE_STRING);
  
}

