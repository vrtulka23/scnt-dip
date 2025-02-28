#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(NodeOptions, Constant) {
  
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string(" !constant");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // constant declaration is not returned as a separate node
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->constant, true); // foo node is set as a constant

  // Throw an error if one is trying to override a constant node
  d = dip::DIP();
  d.add_string("foo bool = true\n !constant\nfoo bool = false");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(NodeOptions, Description) {
  
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string(" !description 'If foo is true, bar is false'");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // description is not returned as a separate node
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->description, "If foo is true, bar is false");

  // Throw an error if one is trying to override a constant node
  // TODO: The test below works only because the group node is the first node in the list.
  //       If there are value nodes before the group node, the description will be assigned
  //       to the first previous value node. This is because description nodes
  //       currently don't see the group nodes in a node list.
  d = dip::DIP();
  d.add_string("foo\n !description 'This is a group node'\n  bar int = 3");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}
