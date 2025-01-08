#include <gtest/gtest.h>

#include "../src/dip.h"

TEST(DIP, Initialization) {

  dip::DIP d;    
  EXPECT_EQ(d.to_string(), "DIP");

  std::string code = "foo str = \"bar\"\ncount int = 3\nnew bool = true";
  d.add_string(code);
  
}
