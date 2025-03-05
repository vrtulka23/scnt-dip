#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Branchig, CaseElseEnd) {

  dip::DIP d;
  d.add_string("@case true");
  d.add_string("  age int = 30");
  d.add_string("@case false");
  d.add_string("  height float = 180");
  d.add_string("@case false");
  d.add_string("  male bool = true");
  d.add_string("@end");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "age");
  
}
