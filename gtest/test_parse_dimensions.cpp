#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(ParseDimensions, Array2D) {
  
  dip::DIP d;
  d.add_string("foo int[2:,3] = [[1,1,0],[0,0,1]]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","1","0","0","0","1"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  EXPECT_EQ(node->dtype, dip::BaseNode::INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,-1},{3,3}}));
  
}

TEST(ParseDimensions, Array3D) {
  
  dip::DIP d;
  d.add_string("foo int[2:,:3,2] = [[[1,2],[3,4],[5,6]],[[7,8],[9,10],[11,12]]]");
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","2","3","4","5","6","7","8","9","10","11","12"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3,2}));
  EXPECT_EQ(node->dtype, dip::BaseNode::INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,-1},{0,3},{2,2}}));
  
}

TEST(ParseDimensions, DimensionSize) {

  dip::DIP d;
  d.add_string("foo int[2] = [[1,2],[3,4]]");  
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Number of value dimensions does not match that of node: 2!=1");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}

TEST(ParseDimensions, ExactDimensions) {
  
  dip::DIP d;
  d.add_string("foo int[2,3] = [[1,2,3],[4,5,6]]");  
  dip::Environment env = d.parse();
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","2","3","4","5","6"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({2,3}));
  
  d = dip::DIP();
  d.add_string("foo int[2,3] = [[1,2,3,4]]");  
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value dimensions do not correspond to the node dimension ranges: [1,4] != [2,3]");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
  
}
  
TEST(ParseDimensions, DimensionRanges) {

  dip::DIP d;
  d.add_string("foo int[:2,2:] = [[1,2,3]]");  
  dip::Environment env = d.parse();
  dip::BaseNode::PointerType node = env.nodes.at(0);
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"1","2","3"}));
  EXPECT_EQ(node->value_shape, dip::BaseValue::ShapeType({1,3}));

  d = dip::DIP();
  d.add_string("foo int[2:,:2] = [[1,2,3]]");  
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value dimensions do not correspond to the node dimension ranges: [1,3] != [2:,:2]");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
  
}
