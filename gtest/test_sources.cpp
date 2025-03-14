#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Sources, CodeAppend) {

  // define scalar
  dip::DIP d;
  d.add_string("$source foo = example_source.dip");
  dip::Environment env = d.parse();
  /*
  EXPECT_EQ(env.nodes.size(), 2);

  dip::BaseNode::PointerType node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  dip::ValueNode::PointerType vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "false");
  */

}
