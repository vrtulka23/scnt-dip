#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Hierarchy, ParentChildNodes) {
  
  dip::DIP d;
  d.add_string("foo int = 2\n  bar int = 3\n bim int = 4\nbom int = 5"); 
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  
  node = env.nodes[1];
  EXPECT_EQ(node->name, "foo.bar");
  
  node = env.nodes[2];
  EXPECT_EQ(node->name, "foo.bim");

  node = env.nodes[3];
  EXPECT_EQ(node->name, "bom");
  
}

TEST(Hierarchy, GroupNodes) {
  
  dip::DIP d;
  d.add_string("foo\n  bar int = 3"); 
  dip::Environment env = d.parse();
  
  dip::BaseNode::PointerType node = env.nodes[0];
  EXPECT_EQ(node->name, "foo.bar");

}
