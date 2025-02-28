#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Modifications, SameDataType) {

  dip::DIP d;
  d.add_string("foo int = 2"); 
  d.add_string("foo int32 = 3"); 
  dip::Environment env = d.parse();

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->value_raw, "2");
  EXPECT_EQ(node->keyword, dip::Node::NODE_INTEGER);
  EXPECT_EQ(node->indent, 0);
  EXPECT_EQ(node->name, "foo");

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_EQ(vnode->value->to_string(), "3");
  EXPECT_EQ(vnode->value->dtype, dip::BaseValue::DataType::INT32);
  
}
  
