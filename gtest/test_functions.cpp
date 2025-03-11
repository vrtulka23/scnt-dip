#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

dip::BaseNode::NodeListType foo(const dip::Environment& env) {
  std::cout << "Hello World!" << std::endl;
  return {}; // TODO: change to a variant of BaseValue or NodeListType...
}

TEST(Functions, BooleanValues) {

  dip::DIP d;
  d.add_function("foo", foo);
  d.add_string("bar bool = (foo)");
  dip::Environment env = d.parse();
  //EXPECT_EQ(env.nodes.size(), 1);
  //std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  //EXPECT_EQ(node->name, "age");

}
