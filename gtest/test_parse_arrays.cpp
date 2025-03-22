#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(ParseArrays, BooleanValue) {
  
  dip::DIP d;
  d.add_string("foo bool[2,3] = [[true,false,true],[true,true,false]]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"true","false","true","true","true","false"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  EXPECT_EQ(node->dtype, dip::BaseNode::BOOLEAN);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,2},{3,3}}));
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "[[true, false, true], [true, true, false]]");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::BOOLEAN);

}

TEST(ParseArrays, IntegerValue) {
  
  dip::DIP d;
  d.add_string("foo int[2,3] = [[1,2,3],[-4,-5,-678910111]]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","2","3","-4","-5","-678910111"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  EXPECT_EQ(node->dtype, dip::BaseNode::INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,2},{3,3}}));
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "[[1, 2, 3], [-4, -5, -678910111]]");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::INTEGER_32);

}

TEST(ParseArrays, FloatValue) {
  
  dip::DIP d;
  d.add_string("foo float[2,3] = [[1,2.2,3.3e3],[-4,-5.5,-6.6e6]]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","2.2","3.3e3","-4","-5.5","-6.6e6"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  EXPECT_EQ(node->dtype, dip::BaseNode::FLOAT);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,2},{3,3}}));
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "[[1.0000, 2.2000, 3300.0], [-4.0000, -5.5000, -6.6000e+06]]");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::FLOAT_64);

}

TEST(ParseArrays, StringValue) {
  
  dip::DIP d;
  d.add_string("foo str[2,3] = [[position,'velo,ci\"ty',\"acce]lera'tion\"],['jerk','snap','crackle']]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"position","velo,ci\"ty","acce]lera'tion","jerk","snap","crackle"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  EXPECT_EQ(node->dtype, dip::BaseNode::STRING);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,2},{3,3}}));
  
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes.at(0));
  EXPECT_EQ(vnode->value->to_string(), "[['position', 'velo,ci\"ty', 'acce]lera'tion'], ['jerk', 'snap', 'crackle']]");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::STRING);

}

TEST(ParseArrays, ArrayToScalarsError) {
  
  dip::DIP d;
  d.add_string("foo int = [1,2,3]");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value size is an array but node is defined as scalar: foo int = [1,2,3]");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}

