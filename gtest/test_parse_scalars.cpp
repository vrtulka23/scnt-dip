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
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw.at(0), "true");
  EXPECT_EQ(node->dtype, dip::BaseNode::BOOLEAN);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "true");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::BOOLEAN);
	
  node = env.nodes.at(1);
  EXPECT_EQ(node->value_raw.at(0), "false");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(1));
  EXPECT_EQ(vnode->value->to_string(), "false");

}

TEST(ParseScalars, IntegerValue) {

  dip::DIP d;
  d.add_string("foo1 int = 23");
  d.add_string("foo2 int = 23456789");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw.at(0), "23");
  EXPECT_EQ(node->dtype, dip::BaseNode::INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "23");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::INTEGER_32);
  
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(1));
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
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw.at(0), "23");
  EXPECT_EQ(node->dtype, dip::BaseNode::FLOAT);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo1");
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "23.000");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::FLOAT_64);
  
  node = env.nodes.at(1);
  EXPECT_EQ(node->value_raw.at(0), "23.456");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_EQ(vnode->value->to_string(), "23.456");
  
  node = env.nodes.at(2);
  EXPECT_EQ(node->value_raw.at(0), "23.456e7");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_EQ(vnode->value->to_string(), "2.3456e+08");

  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(3));
  EXPECT_EQ(vnode->value->to_string(7), "23.123457");

  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(4));
  EXPECT_EQ(vnode->value->to_string(13), "23.123456789123");

  // TODO: This case needs more testing on other platforms
  if (dip::FloatNode::max_float_size==128) {
    vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(5));
    std::cout << dip::FloatNode::max_float_size << std::endl;
    EXPECT_EQ(vnode->value->to_string(18), "23.123456789123456789");
  }

}

TEST(ParseScalars, StringValue) {
  
  dip::DIP d;    
  d.add_string("foo str = \"bar\"");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw.at(0), "bar");
  EXPECT_EQ(node->dtype, dip::BaseNode::STRING);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "bar");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::STRING);
  
}

