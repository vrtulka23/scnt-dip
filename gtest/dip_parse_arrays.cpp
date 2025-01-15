#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/node.h"

TEST(ParseArrays, TestDimensions) {
  
  dip::DIP d;
  d.add_string("foo int[2:,3] = [[1,1,0],[0,0,1]]");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "[[1,1,0],[0,0,1]]");
  EXPECT_EQ(node->keyword, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->dimension, dip::Node::DimensionType({{2,-1},{3,3}}));
  
}

