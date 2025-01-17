#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(ParseTypes, Boolean) {
  
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string("foo bool = false");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "true");
  EXPECT_EQ(node->keyword, dip::Node::NODE_BOOLEAN);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");

  node = env.nodes[1];
  EXPECT_EQ(node->value_raw, "false");

}

TEST(ParseTypes, String) {
  
  dip::DIP d;    
  d.add_string("foo str = \"bar\"");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "bar");
  EXPECT_EQ(node->keyword, dip::Node::NODE_STRING);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  
}

TEST(ParseTypes, Integer) {

  dip::DIP d;
  d.add_string("foo int = 23");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "23");
  EXPECT_EQ(node->keyword, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  
}

TEST(ParseTypes, Float) {

  dip::DIP d;
  d.add_string("foo float = 23");
  d.add_string("foo float = 23.456");
  d.add_string("foo float = 23.456e7");
  dip::Environment env = d.parse();
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "23");
  EXPECT_EQ(node->keyword, dip::Node::NODE_FLOAT);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");
  
  node = env.nodes[1];
  EXPECT_EQ(node->value_raw, "23.456");
  
  node = env.nodes[2];
  EXPECT_EQ(node->value_raw, "23.456e7");
  
}

