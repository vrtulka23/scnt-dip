#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(DIP, Initialization) {

  dip::DIP d;    
  EXPECT_EQ(d.to_string(), "DIP");

  std::string code = "foo str = \"bar\"\ncount int = 3\nnew bool = true";
  d.add_string(code);
  
}

TEST(DIP, Parsing) {

  dip::DIP d;    
  std::string code = "foo str = \"bar\"\n  count int = 3\n # stuff\n   my_group";
  d.add_string(code);
  d.parse();

}
